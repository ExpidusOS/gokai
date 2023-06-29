#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/services/engine-manager.h>
#include <linux/input-event-codes.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Pointer(arguments) {
  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();

  this->cursor = wlr_cursor_create();
  wlr_cursor_attach_input_device(this->cursor, this->getValue());
  wlr_cursor_attach_output_layout(this->cursor, display_manager->getLayout());
  wlr_xcursor_manager_set_cursor_image(input_manager->getXcursorManager(), "left_ptr", this->cursor);

  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_POINTER);

  auto pointer = this->getPointerValue();

  this->button_listener.notify = Pointer::button_handle;
  wl_signal_add(&pointer->events.button, &this->button_listener);

  this->cursor_frame_listener.notify = Pointer::cursor_frame_handle;
  wl_signal_add(&this->cursor->events.frame, &this->cursor_frame_listener);

  this->cursor_axis_listener.notify = Pointer::cursor_axis_handle;
  wl_signal_add(&this->cursor->events.axis, &this->cursor_axis_listener);

  this->cursor_motion_absolute_listener.notify = Pointer::cursor_motion_absolute_handle;
  wl_signal_add(&this->cursor->events.motion_absolute, &this->cursor_motion_absolute_listener);

  this->cursor_motion_listener.notify = Pointer::cursor_motion_handle;
  wl_signal_add(&this->cursor->events.motion, &this->cursor_motion_listener);

  FlutterPointerEvent event = {};
  event.struct_size = sizeof (FlutterPointerEvent);
  event.phase = kAdd;
  event.timestamp = FlutterEngineGetCurrentTime();
  event.device = this->id;
  event.device_kind = kFlutterPointerDeviceKindMouse;

  for (const auto& id : engine_manager->getIds()) {
    auto engine = engine_manager->get(id);
    if (engine == nullptr) continue;

    auto result = FlutterEngineSendPointerEvent(engine->getValue(), &event, 1);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to send pointer event");
    }
  }
}

Pointer::~Pointer() {
  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));

  FlutterPointerEvent event = {};
  event.struct_size = sizeof (FlutterPointerEvent);
  event.phase = kRemove;
  event.timestamp = FlutterEngineGetCurrentTime();
  event.device = this->id;
  event.device_kind = kFlutterPointerDeviceKindMouse;

  for (const auto& id : engine_manager->getIds()) {
    auto engine = engine_manager->get(id);
    if (engine == nullptr) continue;

    auto result = FlutterEngineSendPointerEvent(engine->getValue(), &event, 1);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to send pointer event");
    }
  }

  wl_list_remove(&this->cursor_frame_listener.link);

  wlr_cursor_detach_input_device(this->cursor, this->getValue());
  wlr_cursor_destroy(this->cursor);
}

struct wlr_pointer* Pointer::getPointerValue() {
  return wlr_pointer_from_input_device(this->getValue());
}

struct wlr_cursor* Pointer::getCursor() {
  return this->cursor;
}

void Pointer::button_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, button_listener);
  auto event = reinterpret_cast<struct wlr_pointer_button_event*>(data);

  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  struct wlr_output* output = wlr_output_layout_output_at(display_manager->getLayout(), self->cursor->x, self->cursor->y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev = {};
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.timestamp = FlutterEngineGetCurrentTime();
  ev.device_kind = kFlutterPointerDeviceKindMouse;
  ev.device = self->id;
  ev.x = self->cursor->x;
  ev.y = self->cursor->y;

  if (event->state == WLR_BUTTON_RELEASED) {
    self->buttons.erase(event->button);
    ev.phase = self->buttons.empty() ? kUp : kMove;
  } else {
    ev.phase = self->buttons.empty() ? kDown : kMove;
    self->buttons.insert(event->button);
  }

  ev.buttons = self->getButtons();

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send pointer event");
  }
}

void Pointer::cursor_axis_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_axis_listener);
  auto event = reinterpret_cast<struct wlr_pointer_axis_event*>(data);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  wlr_seat_pointer_notify_axis(
    input_manager->getSeat(),
    event->time_msec, event->orientation, event->delta,
    event->delta_discrete, event->source
  );

  struct wlr_output* output = wlr_output_layout_output_at(display_manager->getLayout(), self->cursor->x, self->cursor->y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev = {};
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = self->buttons.empty() ? kDown : kMove;
  ev.timestamp = FlutterEngineGetCurrentTime();
  ev.device = self->id;
  ev.device_kind = kFlutterPointerDeviceKindMouse;
  ev.signal_kind = kFlutterPointerSignalKindScroll;
  ev.x = self->cursor->x;
  ev.y = self->cursor->y;
  ev.buttons = self->getButtons();

  switch (event->orientation) {
    case WLR_AXIS_ORIENTATION_HORIZONTAL:
      ev.scroll_delta_x = event->delta;
      break;
    case WLR_AXIS_ORIENTATION_VERTICAL:
      ev.scroll_delta_y = event->delta;
      break;
  }

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send pointer event");
  }
}

void Pointer::cursor_frame_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_frame_listener);
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  wlr_seat_pointer_notify_frame(input_manager->getSeat());
}

void Pointer::cursor_motion_absolute_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_motion_absolute_listener);
  auto event = reinterpret_cast<struct wlr_pointer_motion_absolute_event*>(data);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  wlr_cursor_warp_absolute(self->cursor, self->getValue(), event->x, event->y);
  input_manager->setActivePoint(glm::uvec2(self->cursor->x, self->cursor->y));

  struct wlr_output* output = wlr_output_layout_output_at(display_manager->getLayout(), self->cursor->x, self->cursor->y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev = {};
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = self->buttons.empty() ? kHover : kMove;
  ev.timestamp = FlutterEngineGetCurrentTime();
  ev.device = self->id;
  ev.device_kind = kFlutterPointerDeviceKindMouse;
  ev.x = self->cursor->x;
  ev.y = self->cursor->y;
  ev.buttons = self->getButtons();

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send pointer event");
  }
}

void Pointer::cursor_motion_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_motion_listener);
  auto event = reinterpret_cast<struct wlr_pointer_motion_event*>(data);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  wlr_cursor_move(self->cursor, self->getValue(), event->delta_x, event->delta_y);
  input_manager->setActivePoint(glm::uvec2(self->cursor->x, self->cursor->y));

  struct wlr_output* output = wlr_output_layout_output_at(display_manager->getLayout(), self->cursor->x, self->cursor->y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev = {};
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = self->buttons.empty() ? kHover : kMove;
  ev.timestamp = FlutterEngineGetCurrentTime();
  ev.device = self->id;
  ev.device_kind = kFlutterPointerDeviceKindMouse;
  ev.x = self->cursor->x;
  ev.y = self->cursor->y;
  ev.buttons = self->getButtons();

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send pointer event");
  }
}

int64_t Pointer::getButtons() {
  int64_t bitmap = 0;
  for (auto button : this->buttons) {
    switch (button) {
      case BTN_LEFT:
        bitmap |= kFlutterPointerButtonMousePrimary;
        break;
      case BTN_RIGHT:
        bitmap |= kFlutterPointerButtonMouseSecondary;
        break;
      case BTN_MIDDLE:
        bitmap |= kFlutterPointerButtonMouseMiddle;
        break;
      case BTN_BACK:
      case BTN_SIDE:
        bitmap |= kFlutterPointerButtonMouseBack;
        break;
      case BTN_FORWARD:
      case BTN_EXTRA:
        bitmap |= kFlutterPointerButtonMouseForward;
        break;
    }
  }
  return bitmap;
}