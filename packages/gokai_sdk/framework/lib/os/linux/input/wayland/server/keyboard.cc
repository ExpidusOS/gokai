#include <gokai/framework/os/linux/input/wayland/server/keyboard.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Keyboard::Keyboard(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Keyboard(arguments) {
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();
  auto keyboard = this->getKeyboardValue();

  this->context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  this->keymap = xkb_keymap_new_from_names(this->context, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);

  wlr_keyboard_set_keymap(keyboard, this->keymap);
  wlr_keyboard_set_repeat_info(keyboard, 25, 300);

  wlr_seat_set_keyboard(seat, keyboard);
  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_KEYBOARD);

  this->key_listener.notify = Keyboard::key_handle;
  wl_signal_add(&keyboard->events.key, &this->key_listener);

  this->modifiers_listener.notify = Keyboard::modifiers_handle;
  wl_signal_add(&keyboard->events.modifiers, &this->modifiers_listener);
}

Keyboard::~Keyboard() {
  wl_list_remove(&this->key_listener.link);

  xkb_keymap_unref(this->keymap);
  xkb_context_unref(this->context);
}

struct wlr_keyboard* Keyboard::getKeyboardValue() {
  return wlr_keyboard_from_input_device(this->getValue());
}

void Keyboard::key_handle(struct wl_listener* listener, void* data) {
  Keyboard* self = wl_container_of(listener, self, key_listener);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  auto seat = input_manager->getSeat();
  auto keyboard = self->getKeyboardValue();

  wlr_seat_set_keyboard(seat, keyboard);

  auto active_point = input_manager->getActivePoint();

  struct wlr_output* output = wlr_output_layout_output_at(display_manager->getLayout(), active_point.x, active_point.y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;
}

void Keyboard::modifiers_handle(struct wl_listener* listener, void* data) {
  Keyboard* self = wl_container_of(listener, self, modifiers_listener);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();
  auto keyboard = self->getKeyboardValue();

  wlr_seat_set_keyboard(seat, keyboard);
  wlr_seat_keyboard_notify_modifiers(seat, &keyboard->modifiers);
}