#include <gokai/services/input-manager.h>
#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::InputManager"

using namespace Gokai::Services;

InputManager::InputManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getNames") == 0) {
      std::list<std::any> list;
      auto values = this->getNames();
      for (auto value : values) list.push_back(value);
      return this->method_codec.encodeSuccessEnvelope(std::make_any<std::list<std::any>>(list));
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

std::shared_ptr<Gokai::ServiceChannel> InputManager::getServiceChannel() {
  return this->service_channel;
}

std::list<std::string> InputManager::getNames() {
  return std::list<std::string>();
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(std::string name) {
  return nullptr;
}

const std::string InputManager::SERVICE_NAME = "InputManager";