#include <gokai/framework/os/linux/input/wayland/server/pointer.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Pointer(arguments) {
}

struct wlr_pointer* Pointer::getPointerValue() {
  return wlr_pointer_from_input_device(this->getValue());
}