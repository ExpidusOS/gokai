#include <gokai/services/engine-manager.h>
#include <gokai/services/power-manager.h>

#define TAG "Gokai::Services::PowerManager"

using namespace Gokai::Services;

PowerManager::PowerManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getIds") == 0) {
      auto ids = this->getIds();
      std::list<std::any> list;
      for (const auto& id : ids) list.push_back(id.str());
      return this->method_codec.encodeSuccessEnvelope(list);
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });

  this->changed.push_back([this]() {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "changed";
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });
}

std::shared_ptr<Gokai::ServiceChannel> PowerManager::getServiceChannel() {
  return this->service_channel;
}

std::list<xg::Guid> PowerManager::getIds() {
  return std::list<xg::Guid>();
}

std::shared_ptr<Gokai::Devices::Power> PowerManager::get(xg::Guid id) {
  return nullptr;
}

const std::string PowerManager::SERVICE_NAME = "PowerManager";
