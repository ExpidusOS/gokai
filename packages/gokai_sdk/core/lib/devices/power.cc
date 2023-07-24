#include <gokai/devices/power.h>

using namespace Gokai::Devices;

Power::Power(Gokai::ObjectArguments arguments) : Gokai::Object(arguments), id{std::any_cast<xg::Guid>(arguments.get("id"))} {}

xg::Guid Power::getId() {
  return this->id;
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
