#include <gokai/framework/os/linux/input/wayland/server/keyboard.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Keyboard::Keyboard(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Keyboard(arguments) {
}

struct wlr_keyboard* Keyboard::getKeyboardValue() {
  return wlr_keyboard_from_input_device(this->getValue());
}