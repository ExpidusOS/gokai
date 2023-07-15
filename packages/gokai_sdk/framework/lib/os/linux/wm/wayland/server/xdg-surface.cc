#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/wm/wayland/server/xdg-surface.h>

#define TAG "Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface"

using namespace Gokai::Framework::os::Linux::WM::Wayland::Server;

XdgSurface::XdgSurface(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments), id{std::any_cast<xg::Guid>(arguments.get("id"))}, value{std::any_cast<struct wlr_xdg_surface*>(arguments.get("value"))} {
  auto context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;

  this->destroy_listener.notify = XdgSurface::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);

  this->map_listener.notify = XdgSurface::map_handler;
  wl_signal_add(&this->value->events.map, &this->map_listener);

  this->unmap_listener.notify = XdgSurface::unmap_handler;
  wl_signal_add(&this->value->events.unmap, &this->unmap_listener);

  this->value->data = this;
}

XdgSurface::~XdgSurface() {
  for (const auto& func : this->destroy) func();
}

xg::Guid XdgSurface::getId() {
  return this->id;
}

struct wlr_xdg_surface* XdgSurface::getValue() {
  return this->value;
}

void XdgSurface::破壊する(struct wl_listener* listener, void* data) {
  XdgSurface* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}

void XdgSurface::map_handler(struct wl_listener* listener, void* data) {
  XdgSurface* self = wl_container_of(listener, self, map_listener);
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(self->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto win = window_manager->get(self->getId());
  if (win != nullptr) win->setMapped(true);
}

void XdgSurface::unmap_handler(struct wl_listener* listener, void* data) {
  XdgSurface* self = wl_container_of(listener, self, unmap_listener);
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(self->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto win = window_manager->get(self->getId());
  if (win != nullptr) win->setMapped(false);
}