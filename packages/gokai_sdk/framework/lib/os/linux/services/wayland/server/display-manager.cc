#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

void DisplayManager::handle_display_new(struct wl_listener* listener, void* data) {
  DisplayManager* self = wl_container_of(listener, self, display_new);
  self->logger->debug("Output added");
}

DisplayManager::DisplayManager(Gokai::ObjectArguments arguments) : Gokai::Services::DisplayManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  wl_signal_add(&compositor->getBackend()->events.new_output, &this->display_new);
  this->display_new.notify = DisplayManager::handle_display_new;
}