#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor-input-method.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

CompositorInputMethod::CompositorInputMethod(Gokai::ObjectArguments arguments) : Gokai::Services::CompositorInputMethod(arguments), input_method{nullptr} {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  this->text_input_manager = wlr_text_input_manager_v3_create(compositor->getDisplay());
  this->im_manager = wlr_input_method_manager_v2_create(compositor->getDisplay());

  this->text_input_listener.notify = CompositorInputMethod::text_input_handle;
  wl_signal_add(&this->text_input_manager->events.text_input, &this->text_input_listener);

  this->input_method_listener.notify = CompositorInputMethod::input_method_handle;
  wl_signal_add(&this->im_manager->events.input_method, &this->input_method_listener);
}

void CompositorInputMethod::showInput() {
  if (this->input_method != nullptr) {
    this->logger->debug("Activating input method");
    wlr_input_method_v2_send_activate(this->input_method);
    wlr_input_method_v2_send_done(this->input_method);
  }

  this->is_active = true;
}

void CompositorInputMethod::hideInput() {
  if (this->input_method != nullptr) {
    wlr_input_method_v2_send_deactivate(this->input_method);
    this->logger->debug("Deactivating input method");
  }

  this->is_active = false;
}

void CompositorInputMethod::text_input_handle(struct wl_listener* listener, void* data) {
  CompositorInputMethod* self = wl_container_of(listener, self, text_input_listener);
  auto text_input = reinterpret_cast<struct wlr_text_input_v3*>(data);

  if (self->text_input == nullptr) {
    self->text_input = text_input;
  }
}

void CompositorInputMethod::input_method_handle(struct wl_listener* listener, void* data) {
  CompositorInputMethod* self = wl_container_of(listener, self, input_method_listener);
  auto input_method = reinterpret_cast<struct wlr_input_method_v2*>(data);

  if (self->input_method == nullptr) {
    self->input_method = input_method;
  } else {
    wlr_input_method_v2_send_unavailable(input_method);
  }
}