#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/view/wayland/server/display.h>
#include <gokai/services/engine-manager.h>
#include <gokai/view/cairo/image.h>

#define TAG "Gokai::View::Display"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

Display::Display(Gokai::ObjectArguments arguments) : Gokai::View::Display(arguments), Gokai::Loggable(TAG, arguments) {
  this->value = std::any_cast<struct wlr_output*>(arguments.get("value"));
  this->context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));

  this->logger->debug(
    "Display {} ({}) added",
    reinterpret_cast<void*>(this->value),
    this->value->name
  );

  auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
  auto renderer = compositor->getRenderer();
  if (wlr_renderer_is_gles2(renderer)) {
    auto egl = wlr_gles2_renderer_get_egl(renderer);

    this->renderer = new Gokai::Graphics::Rendering::EGL::Renderer(wlr_egl_get_display(egl), wlr_egl_get_context(egl), Gokai::ObjectArguments({
      { "logger", this->getLogger() },
    }));
  } else if (wlr_renderer_is_pixman(renderer)) {
    this->renderer = new Gokai::Graphics::Rendering::Cairo::Renderer(Gokai::ObjectArguments({
      { "logger", this->getLogger() },
      { "size", glm::uvec2(this->value->width, this->value->height) },
    }));
  } else if (wlr_renderer_is_vk(renderer)) {
    this->renderer = new Gokai::Graphics::Rendering::Vulkan::Renderer(Gokai::ObjectArguments({
      { "logger", this->getLogger() },
    }));
  } else {
    throw std::runtime_error(fmt::format("No renderer is available for {}", this->value->name));
  }

  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  this->engine = engine_manager->create(this->renderer);

  this->mode_listener.notify = Display::mode;
  wl_signal_add(&this->value->events.mode, &this->mode_listener);

  this->frame_listener.notify = Display::フレーム;
  wl_signal_add(&this->value->events.frame, &this->frame_listener);

  this->destroy_listener.notify = Display::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Display::~Display() {
  wl_list_remove(&this->mode_listener.link);
  wl_list_remove(&this->frame_listener.link);
  wl_list_remove(&this->destroy_listener.link);

  for (auto func : this->destroy) func();
  delete this->renderer;

  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  engine_manager->destroy(this->engine->getId());

  this->logger->debug("Display {} destroyed", reinterpret_cast<void*>(this->value));
}

void Display::mode(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, mode_listener);

  auto old = self->renderer->getSize();
  try {
    self->engine->resize(glm::uvec2(self->value->width, self->value->height));
  } catch (const std::exception& ex) {
    int32_t refresh = self->value->current_mode != nullptr ? self->value->current_mode->refresh : 0;

    wlr_output_set_custom_mode(self->value, old.x, old.y, refresh);
    auto value = wlr_output_commit(self->value);

    self->logger->error("Failed to resize: {}", ex.what());
    if (!value) {
      wlr_output_enable(self->value, false);
      delete self;
    }
  }
}

void Display::フレーム(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, frame_listener);
  auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(self->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
  auto renderer = compositor->getRenderer();

  wlr_output_attach_render(self->value, nullptr);

  try {
    if (wlr_renderer_is_pixman(renderer)) {
      auto img = wlr_pixman_renderer_get_current_image(renderer);

      auto surface = cairo_image_surface_create_for_data(
        reinterpret_cast<unsigned char*>(pixman_image_get_data(img)),
        CAIRO_FORMAT_ARGB32,
        pixman_image_get_width(img),
        pixman_image_get_height(img),
        pixman_image_get_stride(img)
      );

      auto target = Gokai::View::Cairo::Image(Gokai::ObjectArguments({
        { "logger", self->getLogger() },
        { "value", surface },
      }));

      self->renderer->render(target);
    } else {
      wlr_renderer_begin(renderer, self->value->width, self->value->height);
      wlr_renderer_clear(renderer, (const float[4]){ 0, 0.0, 0, 1.0 });
      wlr_renderer_end(renderer);
    }
  } catch (const std::exception& ex) {
    self->logger->error("Failed to render: {}", ex.what());
  }

  wlr_output_commit(self->value);
}

void Display::破壊する(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}