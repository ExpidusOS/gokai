#include <gokai/framework/os/linux/input/wayland/server/touch.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Touch::Touch(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Touch(arguments) {
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();

  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_TOUCH);
}

struct wlr_touch* Touch::getTouchValue() {
  return wlr_touch_from_input_device(this->getValue());
}