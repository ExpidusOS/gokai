#include <gokai/devices/power.h>
#include <gokai/services/engine-manager.h>

using namespace Gokai::Devices;

Power::Power(Gokai::ObjectArguments arguments) : Gokai::Object(arguments), id{std::any_cast<xg::Guid>(arguments.get("id"))} {
  auto context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;

  this->onChange.push_back([this]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "change";
    call.arguments = this->getId();
    engine_manager->sendAll("Gokai::Services::PowerManager", engine_manager->method_codec.encodeMethodCall(call));
  });
}

xg::Guid Power::getId() {
  return this->id;
}

std::string Power::getName() {
  return "";
}

int Power::getCycleCount() {
  return 0;
}

double Power::getLevel() {
  return 0.0;
}

bool Power::isIntegrated() {
  return false;
}

bool Power::isCharging() {
  return false;
}
