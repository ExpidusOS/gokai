#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>

#define TAG "Gokai::View::Window"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

Window::Window(Gokai::ObjectArguments arguments) : Gokai::View::Window(arguments), Loggable(TAG, arguments), value{std::any_cast<struct wlr_surface*>(arguments.get("value"))} {
  this->context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto pos = input_manager->getActivePoint();

  auto rect = this->getRect();
  rect.pos = pos;
  this->setRect(rect);

  this->commit_listener.notify = Window::commit_handler;
  wl_signal_add(&this->value->events.commit, &this->commit_listener);

  this->destroy_listener.notify = Window::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Window::~Window() {
  for (const auto& func : this->destroy) func();
}

std::string Window::getDisplayName() {
  auto rect = this->getRect();
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  auto output = wlr_output_layout_output_at(display_manager->getLayout(), rect.pos.x, rect.pos.y);
  if (output == nullptr) return nullptr;
  return output->name;
}

void Window::commit_handler(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, commit_listener);
  auto texture = wlr_surface_get_texture(self->value);
}

void Window::破壊する(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}