#include <gokai/framework/os/linux/devices/power.h>

using namespace Gokai::Framework::os::Linux::Devices;

Power::Power(Gokai::ObjectArguments arguments) : Gokai::Devices::Power(arguments), value{std::any_cast<UpDevice*>(arguments.get("value"))} {}

Power::~Power() {
  g_object_unref(G_OBJECT(this->value));
}

UpDevice* Power::getValue() {
  return this->value;
}

int Power::getCycleCount() {
  int value = 0;
  g_object_get(G_OBJECT(this->value), "charge-cycles", &value, nullptr);
  return value;
}

double Power::getLevel() {
  double value;
  double empty;
  double full;

  g_object_get(G_OBJECT(this->value),
    "energy", &value,
    "energy-empty", &empty,
    "energy-full", &full,
    nullptr);

  return (value - empty) / full;
}

bool Power::isIntegrated() {
  guint kind = UP_DEVICE_KIND_UNKNOWN;
  g_object_get(G_OBJECT(this->value), "kind", &kind, nullptr);
  return kind == UP_DEVICE_KIND_BATTERY;
}
