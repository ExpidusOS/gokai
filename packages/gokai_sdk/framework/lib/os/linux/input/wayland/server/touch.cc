#include <gokai/framework/os/linux/input/wayland/server/touch.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/services/engine-manager.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Touch::Touch(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::Input::Wayland::Server::Base(arguments), Gokai::Input::Touch(arguments) {
  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto seat = input_manager->getSeat();

  wlr_seat_set_capabilities(seat, seat->capabilities | WL_SEAT_CAPABILITY_TOUCH);

  auto device = this->getTouchValue();

  this->touch_down_listener.notify = Touch::touch_down_handler;
  wl_signal_add(&device->events.down, &this->touch_down_listener);

  this->touch_up_listener.notify = Touch::touch_up_handler;
  wl_signal_add(&device->events.up, &this->touch_up_listener);

  this->touch_motion_listener.notify = Touch::touch_motion_handler;
  wl_signal_add(&device->events.motion, &this->touch_motion_listener);

  this->touch_cancel_listener.notify = Touch::touch_cancel_handler;
  wl_signal_add(&device->events.cancel, &this->touch_cancel_listener);

  FlutterPointerEvent event = {};
  event.struct_size = sizeof (FlutterPointerEvent);
  event.phase = kAdd;
  event.timestamp = FlutterEngineGetCurrentTime();
  event.device = this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  event.device_kind = kFlutterPointerDeviceKindTouch;

  for (const auto& id : engine_manager->getIds()) {
    auto engine = engine_manager->get(id);
    if (engine == nullptr) continue;

    auto result = FlutterEngineSendPointerEvent(engine->getValue(), &event, 1);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to send touch event");
    }
  }
}

Touch::~Touch() {
  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));

  FlutterPointerEvent event = {};
  event.struct_size = sizeof (FlutterPointerEvent);
  event.phase = kRemove;
  event.timestamp = FlutterEngineGetCurrentTime();
  event.device = this->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  event.device_kind = kFlutterPointerDeviceKindTouch;

  for (const auto& id : engine_manager->getIds()) {
    auto engine = engine_manager->get(id);
    if (engine == nullptr) continue;

    auto result = FlutterEngineSendPointerEvent(engine->getValue(), &event, 1);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to send touch event");
    }
  }

  wl_list_remove(&this->touch_down_listener.link);
  wl_list_remove(&this->touch_up_listener.link);
  wl_list_remove(&this->touch_motion_listener.link);
  wl_list_remove(&this->touch_cancel_listener.link);
}

struct wlr_touch* Touch::getTouchValue() {
  return wlr_touch_from_input_device(this->getValue());
}

void Touch::touch_down_handler(struct wl_listener* listener, void* data) {
  Touch* self = wl_container_of(listener, self, touch_down_listener);
  auto event = reinterpret_cast<struct wlr_touch_down_event*>(data);
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  auto output = wlr_output_layout_output_at(display_manager->getLayout(), event->x, event->y);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev;
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = kDown;
  ev.timestamp = FlutterEngineGetCurrentTime();

  struct wlr_box box;
  wlr_output_layout_get_box(display_manager->getLayout(), nullptr, &box);

  ev.x = event->x * box.width;
  ev.y = event->y * box.height;

  self->last_touch_coords[event->touch_id] = std::pair(event->x, event->y);

  ev.device = self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  ev.device_kind = kFlutterPointerDeviceKindTouch;
	ev.signal_kind = kFlutterPointerSignalKindNone;

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send touch event");
  }
}

void Touch::touch_up_handler(struct wl_listener* listener, void* data) {
  Touch* self = wl_container_of(listener, self, touch_up_listener);
  auto event = reinterpret_cast<struct wlr_touch_up_event*>(data);
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  auto last_touch = self->last_touch_coords[event->touch_id];

  auto output = wlr_output_layout_output_at(display_manager->getLayout(), last_touch.first, last_touch.second);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev;
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = kUp;
  ev.timestamp = FlutterEngineGetCurrentTime();

  struct wlr_box box;
  wlr_output_layout_get_box(display_manager->getLayout(), nullptr, &box);

  ev.x = last_touch.first * box.width;
  ev.y = last_touch.second * box.height;

  ev.device = self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  ev.device_kind = kFlutterPointerDeviceKindTouch;
	ev.signal_kind = kFlutterPointerSignalKindNone;

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send touch event");
  }
}

void Touch::touch_motion_handler(struct wl_listener* listener, void* data) {
  Touch* self = wl_container_of(listener, self, touch_motion_listener);
  auto event = reinterpret_cast<struct wlr_touch_motion_event*>(data);
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  auto last_touch = self->last_touch_coords[event->touch_id];

  auto output = wlr_output_layout_output_at(display_manager->getLayout(), last_touch.first, last_touch.second);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev;
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = kMove;
  ev.timestamp = FlutterEngineGetCurrentTime();

  struct wlr_box box;
  wlr_output_layout_get_box(display_manager->getLayout(), nullptr, &box);

  ev.x = last_touch.first * box.width;
  ev.y = last_touch.second * box.height;

  ev.device = self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  ev.device_kind = kFlutterPointerDeviceKindTouch;
	ev.signal_kind = kFlutterPointerSignalKindNone;

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send touch event");
  }
}

void Touch::touch_cancel_handler(struct wl_listener* listener, void* data) {
  Touch* self = wl_container_of(listener, self, touch_cancel_listener);
  auto event = reinterpret_cast<struct wlr_touch_cancel_event*>(data);
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  auto last_touch = self->last_touch_coords[event->touch_id];

  auto output = wlr_output_layout_output_at(display_manager->getLayout(), last_touch.first, last_touch.second);
  if (output == nullptr) return;

  auto display = display_manager->get(output);
  if (display == nullptr) return;

  FlutterPointerEvent ev;
  ev.struct_size = sizeof (FlutterPointerEvent);
  ev.phase = kCancel;
  ev.timestamp = FlutterEngineGetCurrentTime();

  struct wlr_box box;
  wlr_output_layout_get_box(display_manager->getLayout(), nullptr, &box);

  ev.x = last_touch.first * box.width;
  ev.y = last_touch.second * box.height;

  ev.device = self->Gokai::Framework::os::Linux::Input::Wayland::Server::Base::id;
  ev.device_kind = kFlutterPointerDeviceKindTouch;
	ev.signal_kind = kFlutterPointerSignalKindNone;

  auto result = FlutterEngineSendPointerEvent(display->getEngine()->getValue(), &ev, 1);
  if (result != kSuccess) {
    self->logger->warn("Failed to send touch event");
  }
}
