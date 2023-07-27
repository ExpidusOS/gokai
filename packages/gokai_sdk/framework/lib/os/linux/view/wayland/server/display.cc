#include <gokai/framework/os/linux/graphics/rendering/egl/wayland/server/renderer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/egl/buffer.h>
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
    this->renderer = new Gokai::Framework::os::Linux::Graphics::Rendering::EGL::Wayland::Server::Renderer(Gokai::ObjectArguments({
      { "context", this->context },
      { "value", renderer },
      { "logger", this->getLogger() },
      { "size", glm::uvec2(this->value->width, this->value->height) },
      { "format", this->value->render_format },
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
  this->engine = engine_manager->create(this->renderer, Gokai::Flutter::display, this->getName());

  this->mode_listener.notify = Display::mode;
  wl_signal_add(&this->value->events.mode, &this->mode_listener);

  this->frame_listener.notify = Display::フレーム;
  wl_signal_add(&this->value->events.frame, &this->frame_listener);

  this->destroy_listener.notify = Display::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);

  uv_timer_init(this->context->getLoop(), &this->schedule_frame_handle);
  uv_timer_start(&schedule_frame_handle, [](auto handle) {
    auto self = reinterpret_cast<Display*>((char*)(handle) - offsetof(Display, schedule_frame_handle));
    wlr_output_schedule_frame(self->value);
  }, 0, 0);
}

Display::~Display() {
  wl_list_remove(&this->mode_listener.link);
  wl_list_remove(&this->frame_listener.link);
  wl_list_remove(&this->destroy_listener.link);

  uv_timer_stop(&this->schedule_frame_handle);

  for (auto func : this->destroy) func();
  delete this->renderer;

  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  engine_manager->destroy(this->engine->getId());

  this->logger->debug("Display {} destroyed", reinterpret_cast<void*>(this->value));
}

std::string Display::getName() {
  return std::string(this->value->name);
}

std::string Display::getModel() {
  return std::string(this->value->model);
}

bool Display::isHDR() {
  return false;
}

glm::uvec2 Display::getPhysicalSize() {
  return glm::uvec2(this->value->phys_width, this->value->phys_height);
}

Gokai::View::DisplayMode Display::getMode() {
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  double x;
  double y;
  wlr_output_layout_output_coords(display_manager->getLayout(), this->value, &x, &y);
  glm::uvec2 pos(x * 1, y * 1);

  auto value = Gokai::View::DisplayMode();
  value.resolution = Gokai::View::URect(
    pos,
    glm::uvec2(this->value->width, this->value->height)
  );
  value.refresh = this->value->refresh / 1000;
  return value;
}

std::list<Gokai::View::DisplayMode> Display::getModes() {
  std::list<Gokai::View::DisplayMode> list;
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));

  double x;
  double y;
  wlr_output_layout_output_coords(display_manager->getLayout(), this->value, &x, &y);
  glm::uvec2 pos(x * 1, y * 1);

  struct wlr_output_mode* mode;
  wl_list_for_each(mode, &this->value->modes, link) {
    auto value = Gokai::View::DisplayMode();
    value.resolution = Gokai::View::URect(
      pos,
      glm::uvec2(mode->width, mode->height)
    );
    value.refresh = mode->refresh / 1000;

    list.push_back(value);
  }
  return list;
}

void Display::setMode(Gokai::View::DisplayMode mode) {
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  wlr_output_set_custom_mode(this->value, mode.resolution.size.x, mode.resolution.size.y, mode.refresh * 1000);

  if (!wlr_output_test(this->value)) {
    throw std::runtime_error("Mode failed test");
  }

  if (!wlr_output_commit(this->value)) {
    throw std::runtime_error("Failed to commit");
  }

  wlr_output_layout_move(display_manager->getLayout(), this->value, mode.resolution.pos.x, mode.resolution.pos.y);
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

  if (self->engine->isShutdown()) return;
  self->engine->vsync(
    self->value->refresh != 0
		  ? (double) self->value->refresh / 1000
		  : 60);

  try {
    if (wlr_renderer_is_pixman(renderer)) {
      wlr_output_attach_render(self->value, nullptr);
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
    } else if (wlr_renderer_is_gles2(renderer)) {
      auto render_egl = static_cast<Gokai::Framework::os::Linux::Graphics::Rendering::EGL::Wayland::Server::Renderer*>(self->renderer);

      float mat[9];
      wlr_matrix_identity(mat);

      struct wlr_box box = {
        .x = 0,
        .y = 0,
        .width = self->value->width,
        .height = self->value->height,
      };

      auto transform = (enum wl_output_transform)(self->value->transform | WL_OUTPUT_TRANSFORM_180 | WL_OUTPUT_TRANSFORM_FLIPPED);
      wlr_matrix_project_box(mat, &box, transform, 0, self->value->transform_matrix);

      wlr_output_attach_render(self->value, nullptr);
      wlr_renderer_begin(renderer, self->value->width, self->value->height);
      wlr_renderer_clear(renderer, (const float[4]){ 0, 0.0, 0, 1.0 });
      wlr_render_texture_with_matrix(renderer, render_egl->buffer->getTexture(), mat, 1.0);
      wlr_renderer_end(renderer);

      pixman_region32_t damage;
      std::vector<pixman_box32_t> damage_boxes;
      for (const auto& dmg : render_egl->damage) {
        pixman_box32_t box;
        box.x1 = dmg.pos.x;
        box.y1 = dmg.pos.y;
        box.x2 = dmg.size.x + dmg.pos.x;
        box.y2 = dmg.size.y + dmg.pos.y;
        damage_boxes.push_back(box);
      }

      pixman_region32_init_rects(&damage, damage_boxes.data(), damage_boxes.size());
      wlr_output_set_damage(self->value, &damage);
      render_egl->damage.clear();
      pixman_region32_fini(&damage);
    } else {
      wlr_output_attach_render(self->value, nullptr);
      wlr_renderer_begin(renderer, self->value->width, self->value->height);
      wlr_renderer_clear(renderer, (const float[4]){ 0, 0.0, 0, 1.0 });
      wlr_renderer_end(renderer);
    }
  } catch (const std::exception& ex) {
    self->logger->error("Failed to render: {}", ex.what());
  }

  if (!wlr_output_commit(self->value)) {
    wlr_output_schedule_frame(self->value);
  }
}

void Display::破壊する(struct wl_listener* listener, void* data) {
  Display* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}

struct wlr_output* Display::getValue() {
  return this->value;
}

std::shared_ptr<Gokai::Flutter::Engine> Display::getEngine() {
  return this->engine;
}
