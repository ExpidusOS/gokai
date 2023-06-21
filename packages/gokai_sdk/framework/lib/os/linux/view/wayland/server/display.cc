#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/view/wayland/server/display.h>

#define TAG "Gokai::View::Display"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

Display::Display(Gokai::ObjectArguments arguments) : Gokai::View::Display(arguments), Gokai::Loggable(TAG, arguments) {
  this->value = std::any_cast<struct wlr_output*>(arguments.get("value"));
  this->context = std::any_cast<Context*>(arguments.get("context"));

  this->logger->debug(
    "Display {} ({}) added",
    reinterpret_cast<void*>(this->value),
    this->value->name
  );

  this->frame_listener.notify = Display::frame_handle;
  wl_signal_add(&this->value->events.frame, &this->frame_listener);

  this->destroy_listener.notify = Display::destroy_handle;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Display::~Display() {
  for (auto func : this->destroy) func();
  this->logger->debug("Display {} destroyed", reinterpret_cast<void*>(this->value));
}

void Display::frame_handle(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, frame_listener);
  auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(self->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
  auto renderer = compositor->getRenderer();

  wlr_output_attach_render(self->value, nullptr);
  wlr_renderer_begin(renderer, self->value->width, self->value->height);
  wlr_renderer_clear(renderer, (const float[4]){ 0, 1.0, 0, 1.0 });
  wlr_renderer_end(renderer);
  wlr_output_commit(self->value);
}

void Display::destroy_handle(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}