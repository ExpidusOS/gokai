#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Pointer(arguments) {
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();

  this->cursor = wlr_cursor_create();
  wlr_cursor_attach_input_device(this->cursor, this->getValue());
  wlr_cursor_attach_output_layout(this->cursor, display_manager->getLayout());

  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_KEYBOARD);
}

Pointer::~Pointer() {
  wlr_cursor_detach_input_device(this->cursor, this->getValue());
  wlr_cursor_destroy(this->cursor);
}

struct wlr_pointer* Pointer::getPointerValue() {
  return wlr_pointer_from_input_device(this->getValue());
}

struct wlr_cursor* Pointer::getCursor() {
  return this->cursor;
}