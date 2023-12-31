#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::EngineManager"

using namespace Gokai::Services;

EngineManager::EngineManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    return std::async(std::launch::async, [this, engine_id, channel, message] {
      auto call = this->method_codec.decodeMethodCall(message);

      if (call.method.compare("getEngineId") == 0) {
        return this->method_codec.encodeSuccessEnvelope(engine_id.str());
      }

      if (call.method.compare("getIds") == 0) {
        auto ids = this->getIds();
        std::list<std::any> list;
        for (const auto& id : ids) list.push_back(id.str());
        return this->method_codec.encodeSuccessEnvelope(list);
      }

      if (call.method.compare("getViewType") == 0) {
        auto engine_id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto engine = this->get(engine_id);
        if (engine == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Engine \"{}\" does not exist", engine_id.str()), std::make_any<void*>(nullptr));
        }

        auto type = engine->getViewType();
        switch (type) {
          case Gokai::Flutter::window:
            return this->method_codec.encodeSuccessEnvelope("window");
          case Gokai::Flutter::display:
            return this->method_codec.encodeSuccessEnvelope("display");
        }

        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("\"{}\" is not valid", (uint8_t)type), std::make_any<void*>(nullptr));
      }

      if (call.method.compare("getViewName") == 0) {
        auto engine_id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto engine = this->get(engine_id);
        if (engine == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Engine \"{}\" does not exist", engine_id.str()), std::make_any<void*>(nullptr));
        }

        return this->method_codec.encodeSuccessEnvelope(engine->getViewName());
      }
      return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
    });
  });

  this->changed.push_back([this]() {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "changed";
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });
}

EngineManager::~EngineManager() {}

std::shared_ptr<Gokai::ServiceChannel> EngineManager::getServiceChannel() {
  return this->service_channel;
}

std::shared_ptr<Gokai::Flutter::Engine> EngineManager::create(Gokai::Graphics::Renderer* renderer, Gokai::Flutter::EngineViewType type, std::string name) {
  auto id = xg::newGuid();

  auto engine = std::shared_ptr<Gokai::Flutter::Engine>(new Gokai::Flutter::Engine(Gokai::ObjectArguments({
    { "id", id },
    { "logger", this->getLogger() },
    { "context", this->context },
    { "renderer", renderer },
    { "view-type", type },
    { "view-name", name },
  })));

  engine->destroy.push_back([this, id]() {
    this->engines.erase(id);
    for (const auto& func : this->changed) func();
  });

  this->engines[id] = engine.get();
  for (const auto& func : this->changed) func();
  return engine;
}

void EngineManager::destroy(xg::Guid id) {
  this->engines.erase(id);
}

std::list<xg::Guid> EngineManager::getIds() {
  auto keys = std::list<xg::Guid>();

  for (auto it = this->engines.begin(); it != this->engines.end(); it++) {
    keys.push_back(it->first);
  }

  return keys;
}

Gokai::Flutter::Engine* EngineManager::get(xg::Guid id) {
  auto find = this->engines.find(id);
  if (find == this->engines.end()) {
    return nullptr;
  }

  return find->second;
}

std::map<xg::Guid, std::future<std::vector<uint8_t>>> EngineManager::sendAll(std::string channel, std::vector<uint8_t> data) {
  std::map<xg::Guid, std::future<std::vector<uint8_t>>> value;
  for (const auto& entry : this->engines) {
    value[entry.first] = entry.second->send(channel, data);
  }
  return value;
}

const std::string EngineManager::SERVICE_NAME = "EngineManager";
