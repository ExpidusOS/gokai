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

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getEngineId") == 0) {
      return this->method_codec.encodeSuccessEnvelope(engine_id.str());
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });
}

EngineManager::~EngineManager() {}

std::shared_ptr<Gokai::ServiceChannel> EngineManager::getServiceChannel() {
  return this->service_channel;
}

std::shared_ptr<Gokai::Flutter::Engine> EngineManager::create(Gokai::Graphics::Renderer* renderer) {
  auto id = xg::newGuid();

  auto engine = std::shared_ptr<Gokai::Flutter::Engine>(new Gokai::Flutter::Engine(Gokai::ObjectArguments({
    { "id", id },
    { "logger", this->getLogger() },
    { "context", this->context },
    { "renderer", renderer },
  })));

  this->engines[id] = engine.get();
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
    throw std::invalid_argument("Engines " + id.str() + " does not exist");
  }

  return find->second;
}

std::map<xg::Guid, std::promise<std::vector<uint8_t>*>> EngineManager::sendAll(std::string channel, std::vector<uint8_t> data) {
  std::map<xg::Guid, std::promise<std::vector<uint8_t>*>> value;
  for (const auto& entry : this->engines) {
    value[entry.first] = entry.second->send(channel, data);
  }
  return value;
}

const std::string EngineManager::SERVICE_NAME = "EngineManager";