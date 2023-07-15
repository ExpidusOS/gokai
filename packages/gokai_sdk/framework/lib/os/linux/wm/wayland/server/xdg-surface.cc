#include <gokai/framework/os/linux/wm/wayland/server/xdg-surface.h>

#define TAG "Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface"

using namespace Gokai::Framework::os::Linux::WM::Wayland::Server;

XdgSurface::XdgSurface(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments), id{std::any_cast<xg::Guid>(arguments.get("id"))}, value{std::any_cast<struct wlr_xdg_surface*>(arguments.get("value"))} {
  this->destroy_listener.notify = XdgSurface::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);

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