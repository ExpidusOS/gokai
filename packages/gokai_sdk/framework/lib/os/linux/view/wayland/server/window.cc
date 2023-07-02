#include <gokai/framework/os/linux/view/wayland/server/window.h>

#define TAG "Gokai::View::Window"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

Window::Window(Gokai::ObjectArguments arguments) : Gokai::View::Window(arguments), Loggable(TAG, arguments), value{std::any_cast<struct wlr_surface*>(arguments.get("value"))} {
  this->destroy_listener.notify = Window::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Window::~Window() {
  for (const auto& func : this->destroy) func();
}

void Window::破壊する(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}