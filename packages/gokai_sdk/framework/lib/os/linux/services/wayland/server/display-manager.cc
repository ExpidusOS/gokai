#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

void DisplayManager::handle_display_new(struct wl_listener* listener, void* data) {
  DisplayManager* self = wl_container_of(listener, self, display_new);
  auto value = reinterpret_cast<struct wlr_output*>(data);

  auto compositor = reinterpret_cast<Compositor*>(self->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  wlr_output_init_render(value, compositor->getAllocator(), compositor->getRenderer());

  if (!wl_list_empty(&value->modes)) {
    struct wlr_output_mode* mode = wlr_output_preferred_mode(value);
    wlr_output_set_mode(value, mode);
 		wlr_output_enable(value, true);
 		if (!wlr_output_commit(value)) {
 		  return;
 		}
  }

  try {
    auto display = new Gokai::Framework::os::Linux::View::Wayland::Server::Display(Gokai::ObjectArguments({
      { "context", self->context },
      { "logger", self->getLogger() },
      { "value", value },
    }));
    self->displays.push_back(display);

    display->destroy.push_back([display, self]() {
      self->displays.remove(display);
      for (auto func : self->changed) func();
      if (self->displays.empty()) {
        uv_stop(self->context->getLoop());
      }
    });

    wlr_output_layout_add_auto(self->layout, value);
    for (auto func : self->changed) func();
  } catch (const std::exception& ex) {
    self->logger->error("Failed to activate display \"{}\": {}", value->name, ex.what());
 		wlr_output_enable(value, false);
    if (self->displays.empty()) {
      uv_stop(self->context->getLoop());
    }
  }
}

DisplayManager::DisplayManager(Gokai::ObjectArguments arguments) : Gokai::Services::DisplayManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  this->layout = wlr_output_layout_create();

  wl_signal_add(&compositor->getBackend()->events.new_output, &this->display_new);
  this->display_new.notify = DisplayManager::handle_display_new;
}

struct wlr_output_layout* DisplayManager::getLayout() {
  return this->layout;
}

std::list<std::string> DisplayManager::getNames() {
  std::list<std::string> list;
  for (const auto& display : this->displays) {
    list.push_back(display->getName());
  }
  return list;
}