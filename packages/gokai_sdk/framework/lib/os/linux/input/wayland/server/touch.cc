#include <gokai/framework/os/linux/input/wayland/server/touch.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Touch::Touch(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Touch(arguments), seat{nullptr} {
}

struct wlr_touch* Touch::getTouchValue() {
  return wlr_touch_from_input_device(this->getValue());
}

struct wlr_seat* Touch::getSeat() {
  auto value = Gokai::Framework::os::Linux::Input::Wayland::Server::Base::getSeat();
  if (value == nullptr) {
    if (this->seat == nullptr) {
      auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
      this->seat = wlr_seat_create(compositor->getDisplay(), this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::getName().c_str());
    }
    return this->seat;
  }
  return value;
}