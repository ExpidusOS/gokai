#include <gokai/framework/os/linux/input/wayland/server/switch.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Switch::Switch(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Switch(arguments) {
  auto value = this->getSwitchValue();

  this->toggle_listener.notify = Switch::toggle_handler;
  wl_signal_add(&value->events.toggle, &this->toggle_listener);
}

struct wlr_switch* Switch::getSwitchValue() {
  return wlr_switch_from_input_device(this->getValue());
}

void Switch::toggle_handler(struct wl_listener* listener, void* data) {
  Switch* self = wl_container_of(listener, self, toggle_listener);
  auto event = reinterpret_cast<struct wlr_switch_toggle_event*>(data);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto is_on = event->switch_state == WLR_SWITCH_STATE_ON;

  switch (event->switch_type) {
    case WLR_SWITCH_TYPE_LID:
      for (const auto& func : input_manager->onLid) func(is_on);
      break;
    case WLR_SWITCH_TYPE_TABLET_MODE:
      for (const auto& func : input_manager->onTablet) func(is_on);
      break;
  }
}
