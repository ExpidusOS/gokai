#include <gokai/services/display-manager.h>
#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::DisplayManager"

using namespace Gokai::Services;

DisplayManager::DisplayManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getNames") == 0) {
      std::list<std::any> list;
      auto values = this->getNames();
      for (auto value : values) list.push_back(value);
      return this->method_codec.encodeSuccessEnvelope(std::make_any<std::list<std::any>>(list));
    }

    if (call.method.compare("getModel") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto display = this->get(name);
      if (display == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Display \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }
      return this->method_codec.encodeSuccessEnvelope(display->getModel());
    }

    if (call.method.compare("isHDR") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto display = this->get(name);
      if (display == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Display \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }
      return this->method_codec.encodeSuccessEnvelope(display->isHDR());
    }

    if (call.method.compare("getPhysicalSize") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto display = this->get(name);
      if (display == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Display \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }

      auto vec = display->getPhysicalSize();
      std::list<std::any> list;
      list.push_back(vec.x);
      list.push_back(vec.y);
      return this->method_codec.encodeSuccessEnvelope(list);
    }

    if (call.method.compare("getMode") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto display = this->get(name);
      if (display == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Display \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }

      auto mode = display->getMode();
      std::map<std::string, std::any> map_resolution;
      map_resolution["x"] = mode.resolution.pos.x;
      map_resolution["y"] = mode.resolution.pos.y;
      map_resolution["width"] = mode.resolution.size.x;
      map_resolution["height"] = mode.resolution.size.y;

      std::map<std::string, std::any> map;
      map["resolution"] = map_resolution;
      map["refresh"] = mode.refresh;
      return this->method_codec.encodeSuccessEnvelope(map);
    }

    if (call.method.compare("getModes") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto display = this->get(name);
      if (display == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Display \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }

      auto modes = display->getModes();
      std::list<std::any> list;
      for (const auto& mode : modes) {
        std::map<std::string, std::any> map_resolution;
        map_resolution["x"] = mode.resolution.pos.x;
        map_resolution["y"] = mode.resolution.pos.y;
        map_resolution["width"] = mode.resolution.size.x;
        map_resolution["height"] = mode.resolution.size.y;

        std::map<std::string, std::any> map;
        map["resolution"] = map_resolution;
        map["refresh"] = mode.refresh;

        list.push_back(map);
      }
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

std::shared_ptr<Gokai::ServiceChannel> DisplayManager::getServiceChannel() {
  return this->service_channel;
}

std::list<std::string> DisplayManager::getNames() {
  return std::list<std::string>();
}

Gokai::View::Display* DisplayManager::get(std::string name) {
  return nullptr;
}

const std::string DisplayManager::SERVICE_NAME = "DisplayManager";
