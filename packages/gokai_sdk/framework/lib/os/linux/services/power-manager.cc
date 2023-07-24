#include <gokai/framework/os/linux/services/power-manager.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::Services;

PowerManager::PowerManager(Gokai::ObjectArguments arguments) : Gokai::Services::PowerManager(arguments) {
  this->client = up_client_new();

  if (this->client == nullptr) {
    throw std::runtime_error("Failed to connect to UPower.");
  }

  auto devices = up_client_get_devices(this->client);
  for (gsize i = 0; i < devices->len; i++) {
    auto device = UP_DEVICE(g_ptr_array_index(devices, i));
    PowerManager::device_added(this->client, device, this);
  }
  g_ptr_array_unref(devices);

  this->device_added_id = g_signal_connect(this->client, "device-added", G_CALLBACK(PowerManager::device_added), this);
  this->device_removed_id = g_signal_connect(this->client, "device-removed", G_CALLBACK(PowerManager::device_removed), this);
}

PowerManager::~PowerManager() {
  g_signal_handler_disconnect(this->client, this->device_added_id);
  g_signal_handler_disconnect(this->client, this->device_removed_id);

  g_clear_object(&this->client);
}

std::list<xg::Guid> PowerManager::getIds() {
  std::list<xg::Guid> list;
  for (const auto& value : this->devices) list.push_back(value->getId());
  return list;
}

std::shared_ptr<Gokai::Devices::Power> PowerManager::get(xg::Guid id) {
  for (const auto& value : this->devices) {
    if (value->getId() == id) return value;
  }
  return nullptr;
}


void PowerManager::device_added(UpClient* client, UpDevice* device, gpointer data) {
  auto self = reinterpret_cast<PowerManager*>(data);

  int kind = UP_DEVICE_KIND_UNKNOWN;
  g_object_get(G_OBJECT(device), "kind", &kind, nullptr);
  if (kind == UP_DEVICE_KIND_MONITOR) return;

  self->devices.push_back(std::shared_ptr<Gokai::Framework::os::Linux::Devices::Power>(new Gokai::Framework::os::Linux::Devices::Power(Gokai::ObjectArguments({
    { "value", device },
    { "id", xg::newGuid() },
  }))));
  for (const auto& func : self->changed) func();
}

void PowerManager::device_removed(UpClient* client, UpDevice* device, gpointer data) {
  auto self = reinterpret_cast<PowerManager*>(data);

  auto device_obj_path = up_device_get_object_path(device);

  for (const auto& dev : self->devices) {
    auto dev_obj_path = up_device_get_object_path(dev->getValue());
    if (strcmp(dev_obj_path, device_obj_path) == 0) {
      self->devices.remove(dev);
      for (const auto& func : self->changed) func();
      break;
    }
  }
}
