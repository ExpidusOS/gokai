#include <gokai/flutter/codecs/json.h>
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
  this->state = xkb_state_new(this->keymap);

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

  xkb_state_unref(this->state);
  xkb_keymap_unref(this->keymap);
  xkb_context_unref(this->context);
}

struct wlr_keyboard* Keyboard::getKeyboardValue() {
  return wlr_keyboard_from_input_device(this->getValue());
}

void Keyboard::key_handle(struct wl_listener* listener, void* data) {
  Keyboard* self = wl_container_of(listener, self, key_listener);
  auto event = reinterpret_cast<struct wlr_keyboard_key_event*>(data);

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

  xkb_keycode_t scan_code = event->keycode + 8;
  xkb_keysym_t keysym = xkb_state_key_get_one_sym(self->state, scan_code);
  uint32_t unicode = xkb_state_key_get_utf32(self->state, scan_code);
  auto modifiers = wlr_keyboard_get_modifiers(keyboard);

  auto ev = std::map<std::string, std::any>();
  ev["keymap"] = "linux";
  ev["toolkit"] = "gtk";
  ev["scanCode"] = scan_code;
  ev["keyCode"] = keysym;
  ev["unicodeScalarValues"] = unicode;

  if (keysym < 128) {
    ev["specifiedLogicalKey"] = keysym;
  }

  ev["modifiers"] = modifiers;

  switch (event->state) {
    case WL_KEYBOARD_KEY_STATE_PRESSED:
      ev["type"] = "keydown";
      break;
    case WL_KEYBOARD_KEY_STATE_RELEASED:
      ev["type"] = "keyup";
      break;
  }

  std::thread([self, display, ev, event, seat]() {
    auto codec = Gokai::Flutter::Codecs::JSONMessageCodec(Gokai::ObjectArguments({}));
    auto future = display->getEngine()->send("flutter/keyevent", codec.encodeMessage(ev));
    future.wait();
    auto response_value = future.get();

    if (response_value.size() > 0) {
      try {
        auto response = std::any_cast<std::map<std::string, std::any>>(codec.decodeMessage(response_value));
        auto handled = std::any_cast<bool>(response["handled"]);

        if (!handled) {
          wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
        }
      } catch (const std::exception& ex) {
        self->logger->error("Failed to read response: {}", ex.what());
        wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
      }
    } else {
      wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
    }
  }).detach();
}

void Keyboard::modifiers_handle(struct wl_listener* listener, void* data) {
  Keyboard* self = wl_container_of(listener, self, modifiers_listener);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();
  auto keyboard = self->getKeyboardValue();

  wlr_seat_set_keyboard(seat, keyboard);
  wlr_seat_keyboard_notify_modifiers(seat, &keyboard->modifiers);
}