#include <gokai/framework/os/linux/input/wayland/server/touch.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Touch::Touch(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Touch(arguments) {
}

struct wlr_touch* Touch::getTouchValue() {
  return wlr_touch_from_input_device(this->getValue());
}