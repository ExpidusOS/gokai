#include <gokai/framework/os/linux/input/wayland/server/keyboard.h>
#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/input/wayland/server/switch.h>
#include <gokai/framework/os/linux/input/wayland/server/touch.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>
#include <assert.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

void InputManager::handle_input_new(struct wl_listener* listener, void* data) {
  InputManager* self = wl_container_of(listener, self, input_new);
  auto value = reinterpret_cast<struct wlr_input_device*>(data);
  auto id = xg::newGuid();

  Gokai::Framework::os::Linux::Input::Wayland::Server::Base* input = nullptr;

  switch (value->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
      try {
        input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Keyboard(Gokai::ObjectArguments({
          { "id", id },
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create keyboard: {}", ex.what());
        return;
      }
      break;
    case WLR_INPUT_DEVICE_POINTER:
      try {
        input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Pointer(Gokai::ObjectArguments({
          { "id", id },
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create pointer: {}", ex.what());
        return;
      }
      break;
    case WLR_INPUT_DEVICE_SWITCH:
      try {
        input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Switch(Gokai::ObjectArguments({
          { "id", id },
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create switch: {}", ex.what());
        return;
      }
      break;
    case WLR_INPUT_DEVICE_TOUCH:
      try {
        input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Touch(Gokai::ObjectArguments({
          { "id", id },
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create touch: {}", ex.what());
        return;
      }
      break;
    default:
      self->logger->warn("Unknown device type {}", (uint16_t)value->type);
      return;
  }

  assert(input != nullptr);
  input->destroy.push_back([input, id, self]() {
    self->inputs.erase(id);
    for (auto func : self->changed) func();
  });

  self->inputs[id] = input;
  for (auto func : self->changed) func();
}

void InputManager::handle_cursor_request(struct wl_listener* listener, void* data) {
  InputManager* self = wl_container_of(listener, self, cursor_request);
  auto event = reinterpret_cast<struct wlr_seat_pointer_request_set_cursor_event*>(data);

  for (const auto& input : self->inputs) {
    auto pointer = dynamic_cast<Gokai::Framework::os::Linux::Input::Wayland::Server::Pointer*>(input.second);
    if (pointer == nullptr) continue;

    wlr_cursor_set_surface(pointer->getCursor(), event->surface, event->hotspot_x, event->hotspot_y);
  }
}

InputManager::InputManager(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Services::InputManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  wlr_data_control_manager_v1_create(compositor->getDisplay());

  this->seat = wlr_seat_create(compositor->getDisplay(), "default");
  this->cursor_request.notify = InputManager::handle_cursor_request;
  wl_signal_add(&this->seat->events.request_set_cursor, &this->cursor_request);

  this->xcursor_manager = wlr_xcursor_manager_create("default", 24);
  wlr_xcursor_manager_load(this->xcursor_manager, 1);

  this->input_new.notify = InputManager::handle_input_new;
  wl_signal_add(&compositor->getBackend()->events.new_input, &this->input_new);
}

struct wlr_xcursor_manager* InputManager::getXcursorManager() {
  return this->xcursor_manager;
}

struct wlr_seat* InputManager::getSeat() {
  return this->seat;
}

std::list<xg::Guid> InputManager::getIds() {
  auto list = Gokai::Framework::os::Linux::Services::InputManager::getIds();
  for (const auto& entry : this->inputs) {
    list.push_back(entry.second->getId());
  }
  return list;
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(xg::Guid id) {
  auto value = Gokai::Framework::os::Linux::Services::InputManager::get(id);
  if (value != nullptr) return value;

  auto found = this->inputs.find(id);
  if (found == this->inputs.end()) return nullptr;
  return std::shared_ptr<Gokai::Input::Base>(found->second);
}

glm::uvec2 InputManager::getActivePoint() {
  return this->active_point;
}

void InputManager::setActivePoint(glm::uvec2 point) {
  this->active_point = point;
}

bool InputManager::sendPointer(Gokai::Input::Pointer::Event event) {
  auto time = FlutterEngineGetCurrentTime() / 1'000'000;

  if (event.type == Gokai::Input::Pointer::EventType::hover) {
    auto window_manager = reinterpret_cast<WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
    auto window = window_manager->get(event.window_id);
    if (window == nullptr) return false;

    wlr_seat_pointer_notify_enter(this->seat, reinterpret_cast<Gokai::Framework::os::Linux::View::Wayland::Server::Window*>(window)->getValue(), event.pos.x, event.pos.y);
    wlr_seat_pointer_notify_motion(this->seat, time, event.pos.x, event.pos.y);
    return true;
  } else if (event.type == Gokai::Input::Pointer::EventType::button) {
    int button = 0;

    switch (event.button) {
      case kFlutterPointerButtonMousePrimary:
        button = BTN_LEFT;
        break;
      case kFlutterPointerButtonMouseSecondary:
        button = BTN_RIGHT;
        break;
      case kFlutterPointerButtonMouseMiddle:
        button = BTN_MIDDLE;
        break;
      default:
        return false;
    }

    wlr_seat_pointer_notify_button(this->seat, time, button, event.is_released ? WLR_BUTTON_RELEASED : WLR_BUTTON_PRESSED);
    wlr_seat_pointer_notify_frame(this->seat);
    return true;
  } else if (event.type == Gokai::Input::Pointer::EventType::leave) {
    wlr_seat_pointer_notify_clear_focus(this->seat);
    return true;
  }
  return false;
}

bool InputManager::sendTouch(Gokai::Input::Touch::Event event) {
  auto time = FlutterEngineGetCurrentTime() / 1'000'000;

  if (event.type == Gokai::Input::Touch::EventType::up) {
    wlr_seat_touch_notify_up(this->seat, time, event.id);
    wlr_seat_touch_notify_frame(this->seat);
    return true;
  } else if (event.type == Gokai::Input::Touch::EventType::down) {
    auto window_manager = reinterpret_cast<WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
    auto window = window_manager->get(event.window_id);
    if (window == nullptr) return false;

    wlr_seat_touch_notify_down(this->seat, reinterpret_cast<Gokai::Framework::os::Linux::View::Wayland::Server::Window*>(window)->getValue(), time, event.id, event.pos.x, event.pos.y);
    wlr_seat_touch_notify_frame(this->seat);
    return true;
  } else if (event.type == Gokai::Input::Touch::EventType::motion) {
    wlr_seat_touch_notify_motion(this->seat, time, event.id, event.pos.x, event.pos.y);
    wlr_seat_touch_notify_frame(this->seat);
    return true;
  } else if (event.type == Gokai::Input::Touch::EventType::cancel) {
    auto window_manager = reinterpret_cast<WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
    auto window = window_manager->get(event.window_id);
    if (window == nullptr) return false;

    wlr_seat_touch_notify_cancel(this->seat, reinterpret_cast<Gokai::Framework::os::Linux::View::Wayland::Server::Window*>(window)->getValue());
    wlr_seat_touch_notify_frame(this->seat);
    return true;
  }
  return false;
}

bool InputManager::sendText(xg::Guid window_id, std::string text) {
  return false;
}

bool InputManager::sendKeyboard(xg::Guid window_id, int keycode) {
  auto time = FlutterEngineGetCurrentTime() / 1'000'000;
  wlr_seat_keyboard_notify_key(this->seat, time, keycode, WL_KEYBOARD_KEY_STATE_PRESSED);
  wlr_seat_keyboard_notify_key(this->seat, time, keycode, WL_KEYBOARD_KEY_STATE_RELEASED);
  return true;
}
