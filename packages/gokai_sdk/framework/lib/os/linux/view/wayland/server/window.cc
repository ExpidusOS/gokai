#include <gokai/framework/os/linux/view/wayland/server/window.h>

#define TAG "Gokai::View::Window"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

Window::Window(Gokai::ObjectArguments arguments) : Gokai::View::Window(arguments), Loggable(TAG, arguments), value{std::any_cast<struct wlr_surface*>(arguments.get("value"))} {
}

Window::~Window() {
  for (const auto& func : this->destroy) func();
}