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

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    return std::async(std::launch::async, [this, engine_id, channel, message] {
      auto call = this->method_codec.decodeMethodCall(message);

      if (call.method.compare("getIds") == 0) {
        std::list<std::any> list;
        auto values = this->getIds();
        for (auto value : values) list.push_back(value.str());
        return this->method_codec.encodeSuccessEnvelope(std::make_any<std::list<std::any>>(list));
      }

      if (call.method.compare("sendPointer") == 0) {
        auto event = Gokai::Input::Pointer::Event(call.arguments);
        return this->method_codec.encodeSuccessEnvelope(this->sendPointer(event));
      }

      if (call.method.compare("sendTouch") == 0) {
        auto event = Gokai::Input::Touch::Event(call.arguments);
        return this->method_codec.encodeSuccessEnvelope(this->sendTouch(event));
      }

      if (call.method.compare("sendText") == 0) {
        auto map = std::any_cast<std::map<std::string, std::any>>(call.arguments);

        return this->method_codec.encodeSuccessEnvelope(this->sendText(
          xg::Guid(std::any_cast<std::string>(map["windowId"])),
          std::any_cast<std::string>(map["text"])
        ));
      }

      if (call.method.compare("sendKeyboard") == 0) {
        auto map = std::any_cast<std::map<std::string, std::any>>(call.arguments);

        return this->method_codec.encodeSuccessEnvelope(this->sendKeyboard(
          xg::Guid(std::any_cast<std::string>(map["windowId"])),
          std::any_cast<int>(map["keycode"])
        ));
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

  this->onLid.push_back([this](auto value) {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "lid";
    call.arguments = value;
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });

  this->onTablet.push_back([this](auto value) {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "tablet";
    call.arguments = value;
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });
}

std::shared_ptr<Gokai::ServiceChannel> InputManager::getServiceChannel() {
  return this->service_channel;
}

std::list<xg::Guid> InputManager::getIds() {
  return std::list<xg::Guid>();
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(xg::Guid id) {
  return nullptr;
}

glm::uvec2 InputManager::getActivePoint() {
  return glm::uvec2();
}

bool InputManager::sendPointer(Gokai::Input::Pointer::Event event) {
  return false;
}

bool InputManager::sendTouch(Gokai::Input::Touch::Event event) {
  return false;
}

bool InputManager::sendText(xg::Guid window_id, std::string text) {
  return false;
}

bool InputManager::sendKeyboard(xg::Guid window_id, int keycode) {
  return false;
}

const std::string InputManager::SERVICE_NAME = "InputManager";
