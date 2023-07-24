#include <gokai/framework/os/linux/devices/power.h>

using namespace Gokai::Framework::os::Linux::Devices;

Power::Power(Gokai::ObjectArguments arguments) : Gokai::Devices::Power(arguments), value{std::any_cast<UpDevice*>(arguments.get("value"))} {
  this->energy_id = g_signal_connect(this->value, "notify::energy", G_CALLBACK(Power::energy_callback), this);
  this->state_id = g_signal_connect(this->value, "notify::state", G_CALLBACK(Power::state_callback), this);
}

Power::~Power() {
  g_signal_handler_disconnect(this->value, this->energy_id);
  g_signal_handler_disconnect(this->value, this->state_id);
  g_clear_object(&this->value);
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

bool Power::isCharging() {
  guint state = UP_DEVICE_STATE_UNKNOWN;
  g_object_get(this->value, "state", &state, nullptr);
  return state == UP_DEVICE_STATE_CHARGING || state == UP_DEVICE_STATE_PENDING_CHARGE;
}

void Power::energy_callback(GObject* obj, GParamSpec* pspec, gpointer data) {
  auto self = reinterpret_cast<Power*>(data);
  for (const auto& func : self->onChange) func();
}

void Power::state_callback(GObject* obj, GParamSpec* pspec, gpointer data) {
  auto self = reinterpret_cast<Power*>(data);
  for (const auto& func : self->onChange) func();
}
