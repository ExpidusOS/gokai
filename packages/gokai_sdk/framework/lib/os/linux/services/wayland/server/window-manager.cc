#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Gokai::Services::WindowManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  this->compositor = wlr_compositor_create(compositor->getDisplay(), compositor->getRenderer());
  this->subcompositor = wlr_subcompositor_create(compositor->getDisplay());

  this->xdg_shell = wlr_xdg_shell_create(compositor->getDisplay(), xdg_wm_base_interface.version);

  this->new_surface_listener.notify = WindowManager::new_surface_handle;
  wl_signal_add(&this->compositor->events.new_surface, &this->new_surface_listener);
}

std::list<xg::Guid> WindowManager::getIds() {
  std::list<xg::Guid> list;
  for (const auto& entry : this->windows) list.push_back(entry.first);
  return list;
}

Gokai::View::Window* WindowManager::get(xg::Guid id) {
  auto find = this->windows.find(id);
  if (find == this->windows.end()) return nullptr;
  return find->second;
}

struct wlr_compositor* WindowManager::getCompositor() {
  return this->compositor;
}

struct wlr_subcompositor* WindowManager::getSubcompositor() {
  return this->subcompositor;
}

struct wlr_xdg_shell* WindowManager::getXdgShell() {
  return this->xdg_shell;
}

void WindowManager::new_surface_handle(struct wl_listener* listener, void* data) {
  WindowManager* self = wl_container_of(listener, self, new_surface_listener);
  auto surface = reinterpret_cast<struct wlr_surface*>(data);
  auto id = xg::newGuid();

  auto window = new Gokai::Framework::os::Linux::View::Wayland::Server::Window(Gokai::ObjectArguments({
    { "logger", self->getLogger() },
    { "id", id },
    { "context", self->context },
    { "value", surface },
  }));

  window->destroy.push_back([self, id]() {
    self->windows.erase(id);
    for (const auto& func : self->changed) func();
  });

  self->windows[id] = window;
  for (const auto& func : self->changed) func();
}