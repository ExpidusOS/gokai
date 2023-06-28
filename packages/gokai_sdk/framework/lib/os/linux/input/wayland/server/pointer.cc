#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/services/engine-manager.h>

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

  FlutterPointerEvent event = {};
  event.struct_size = sizeof (FlutterPointerEvent);
  event.phase = kAdd;
  event.timestamp = FlutterEngineGetCurrentTime();
  event.device = this->id;

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
  Pointer* self = wl_container_of(listener, self, cursor_frame_listener);
  auto event = reinterpret_cast<struct wlr_pointer_button_event*>(data);
}

void Pointer::cursor_axis_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_frame_listener);
  auto event = reinterpret_cast<struct wlr_pointer_axis_event*>(data);

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  wlr_seat_pointer_notify_axis(
    input_manager->getSeat(),
    event->time_msec, event->orientation, event->delta,
    event->delta_discrete, event->source
  );
}

void Pointer::cursor_frame_handle(struct wl_listener* listener, void* data) {
  Pointer* self = wl_container_of(listener, self, cursor_frame_listener);
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  wlr_seat_pointer_notify_frame(input_manager->getSeat());
}