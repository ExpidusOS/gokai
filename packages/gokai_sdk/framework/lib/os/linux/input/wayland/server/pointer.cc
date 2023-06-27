#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Pointer(arguments), seat{nullptr} {
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  this->cursor = wlr_cursor_create();
  wlr_cursor_attach_input_device(this->cursor, this->getValue());
  wlr_cursor_attach_output_layout(this->cursor, display_manager->getLayout());
}

Pointer::~Pointer() {
  wlr_cursor_detach_input_device(this->cursor, this->getValue());
  wlr_cursor_destroy(this->cursor);

  if (this->seat != nullptr) {
    wlr_seat_destroy(this->seat);
    this->seat = nullptr;
  }
}

struct wlr_pointer* Pointer::getPointerValue() {
  return wlr_pointer_from_input_device(this->getValue());
}

struct wlr_seat* Pointer::getSeat() {
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

struct wlr_cursor* Pointer::getCursor() {
  return this->cursor;
}