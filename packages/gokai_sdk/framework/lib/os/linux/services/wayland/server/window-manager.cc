#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>
#include <gokai/services/engine-manager.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Gokai::Services::WindowManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  this->compositor = wlr_compositor_create(compositor->getDisplay(), compositor->getRenderer());
  this->subcompositor = wlr_subcompositor_create(compositor->getDisplay());

  this->decoration_manager = wlr_xdg_decoration_manager_v1_create(compositor->getDisplay());

  this->new_toplevel_decoration_listener.notify = WindowManager::new_toplevel_decoration_handle;
  wl_signal_add(&this->decoration_manager->events.new_toplevel_decoration, &this->new_toplevel_decoration_listener);

  this->xdg_shell = wlr_xdg_shell_create(compositor->getDisplay(), xdg_wm_base_interface.version);

  this->new_xdg_surface_listener.notify = WindowManager::new_xdg_surface_handle;
  wl_signal_add(&this->xdg_shell->events.new_surface, &this->new_xdg_surface_listener);

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

Gokai::Framework::os::Linux::WM::Wayland::Server::XdgDecoration* WindowManager::getXdgDecoration(xg::Guid id) {
  auto find = this->xdg_decorations.find(id);
  if (find == this->xdg_decorations.end()) return nullptr;
  return find->second;
}

Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface* WindowManager::getXdg(xg::Guid id) {
  auto find = this->xdg_surfaces.find(id);
  if (find == this->xdg_surfaces.end()) return nullptr;
  return find->second;
}

struct wlr_compositor* WindowManager::getCompositor() {
  return this->compositor;
}

struct wlr_subcompositor* WindowManager::getSubcompositor() {
  return this->subcompositor;
}

struct wlr_xdg_decoration_manager_v1* WindowManager::getDecorationManager() {
  return this->decoration_manager;
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

  window->onCommit.push_back([self, id]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(self->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "commit";
    call.arguments = id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", self->method_codec.encodeMethodCall(call));
  });

  window->destroy.push_back([self, id]() {
    self->windows.erase(id);
    for (const auto& func : self->changed) func();
  });

  self->windows[id] = window;
  for (const auto& func : self->changed) func();
}

void WindowManager::new_xdg_surface_handle(struct wl_listener* listener, void* data) {
  WindowManager* self = wl_container_of(listener, self, new_xdg_surface_listener);
  auto value = reinterpret_cast<struct wlr_xdg_surface*>(data);

  auto window = static_cast<Gokai::View::Window*>(value->surface->data);
  auto id = window->getId();

  auto surface = new Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface(Gokai::ObjectArguments({
    { "logger", self->getLogger() },
    { "id", id },
    { "context", self->context },
    { "value", value },
  }));

  surface->destroy.push_back([self, id]() {
    self->xdg_surfaces.erase(id);
    for (const auto& func : self->changed) func();
  });

  self->xdg_surfaces[id] = surface;
  for (const auto& func : self->changed) func();
}

void WindowManager::new_toplevel_decoration_handle(struct wl_listener* listener, void* data) {
  WindowManager* self = wl_container_of(listener, self, new_toplevel_decoration_listener);
  auto value = reinterpret_cast<struct wlr_xdg_toplevel_decoration_v1*>(data);

  auto window = static_cast<Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface*>(value->surface->data);
  auto id = window->getId();

  auto decoration = new Gokai::Framework::os::Linux::WM::Wayland::Server::XdgDecoration(Gokai::ObjectArguments({
    { "logger", self->getLogger() },
    { "id", id },
    { "context", self->context },
    { "value", value },
  }));

  decoration->destroy.push_back([self, id]() {
    self->windows.erase(id);
    for (const auto& func : self->changed) func();
  });

  self->xdg_decorations[id] = decoration;
  for (const auto& func : self->changed) func();
}