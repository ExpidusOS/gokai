#include <gokai/framework/os/linux/input/wayland/server/keyboard.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Keyboard::Keyboard(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Keyboard(arguments) {
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();

  wlr_seat_set_keyboard(seat, this->getKeyboardValue());
  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_KEYBOARD);
}

Keyboard::~Keyboard() {
}

struct wlr_keyboard* Keyboard::getKeyboardValue() {
  return wlr_keyboard_from_input_device(this->getValue());
}