#include <gokai/framework/os/linux/wm/wayland/server/xdg-decoration.h>

#define TAG "Gokai::Framework::os::Linux::WM::Wayland::Server::XdgDecoration"

using namespace Gokai::Framework::os::Linux::WM::Wayland::Server;

XdgDecoration::XdgDecoration(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments), id{std::any_cast<xg::Guid>(arguments.get("id"))}, value{std::any_cast<struct wlr_xdg_toplevel_decoration_v1*>(arguments.get("value"))} {
  this->is_client_side = false;

  this->destroy_listener.notify = XdgDecoration::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);

  this->mode_listener.notify = XdgDecoration::mode_handle;
  wl_signal_add(&this->value->events.request_mode, &this->mode_listener);
}

XdgDecoration::~XdgDecoration() {
  for (const auto& func : this->destroy) func();
}

bool XdgDecoration::isClientSide() {
  return this->is_client_side;
}

xg::Guid XdgDecoration::getId() {
  return this->id;
}

struct wlr_xdg_toplevel_decoration_v1* XdgDecoration::getValue() {
  return this->value;
}

void XdgDecoration::破壊する(struct wl_listener* listener, void* data) {
  XdgDecoration* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}

void XdgDecoration::mode_handle(struct wl_listener* listener, void* data) {
  XdgDecoration* self = wl_container_of(listener, self, mode_listener);

  if (self->value->requested_mode != WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_NONE) {
    wlr_xdg_toplevel_decoration_v1_set_mode(self->value, self->value->requested_mode);
    self->is_client_side = self->value->requested_mode == WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;
  } else {
    wlr_xdg_toplevel_decoration_v1_set_mode(self->value, WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    self->is_client_side = false;
  }
}