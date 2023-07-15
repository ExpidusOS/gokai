#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <stdio.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

static struct wlr_renderer* renderer_from_string(std::string name, struct wlr_backend* backend) {
  if (name.compare("auto") == 0) {
    return wlr_renderer_autocreate(backend);
  } else if (name.compare("pixman") == 0) {
    return wlr_pixman_renderer_create();
  } else if (name.compare("gles2") == 0) {
    return wlr_gles2_renderer_create_with_drm_fd(wlr_backend_get_drm_fd(backend));
  } else if (name.compare("vulkan") == 0) {
     return wlr_vk_renderer_create_with_drm_fd(wlr_backend_get_drm_fd(backend));
  }
  throw std::runtime_error("Renderer \"" + name + "\" is either not supported or does not exist");
}

static spdlog::level::level_enum wlr2spdlog(enum wlr_log_importance level) {
  switch (level) {
    case WLR_DEBUG:
      return spdlog::level::level_enum::debug;
    case WLR_INFO:
      return spdlog::level::level_enum::info;
    case WLR_ERROR:
      return spdlog::level::level_enum::err;
    case WLR_SILENT:
      return spdlog::level::level_enum::off;
    default:
      throw std::runtime_error("Incompatible level");
  }
}

static enum wlr_log_importance spdlog2wlr(spdlog::level::level_enum level) {
  switch (level) {
    case spdlog::level::level_enum::trace:
    case spdlog::level::level_enum::debug:
      return WLR_DEBUG;
    case spdlog::level::level_enum::info:
      return WLR_INFO;
    case spdlog::level::level_enum::warn:
    case spdlog::level::level_enum::err:
    case spdlog::level::level_enum::critical:
      return WLR_ERROR;
    case spdlog::level::level_enum::off:
      return WLR_SILENT;
    default:
      throw std::runtime_error("Incompatible level");
  }
}

static void spdlog4wlr(enum wlr_log_importance importance, const char* fmt, va_list args) {
  auto logger = spdlog::get("Gokai::Services::Compositor");

  auto level = wlr2spdlog(importance);
  char* msg = nullptr;
  if (vasprintf(&msg, fmt, args) == -1) {
    logger->error("vasprintf failed.");
    return;
  }

  logger->log(level, msg);
  free(msg);
}

void Compositor::poll_event_handle(uv_poll_t* event_poll, int status, int events) {
  Compositor* self = wl_container_of(event_poll, self, event_poll);

  auto event_loop = wl_display_get_event_loop(self->display);
  wl_display_flush_clients(self->display);
  wl_event_loop_dispatch(event_loop, -1);
}

Compositor::Compositor(Gokai::ObjectArguments arguments) : Gokai::Services::Compositor(arguments) {
  wlr_log_init(spdlog2wlr(this->logger->level()), spdlog4wlr);

  auto manifest = this->context->getManifest();

  this->display = wl_display_create();

  this->backend = wlr_backend_autocreate(this->display);
  if (this->backend == nullptr) {
    throw std::runtime_error("Failed to create wlroots backend");
  }

  auto find = manifest.defaults.find("Gokai::Services::Compositor::renderer");
  if (find != manifest.defaults.end()) {
    auto override_find = manifest.overrides.find("Gokai::Services::Compositor::renderer");
    if (override_find != manifest.overrides.end()) {
      this->renderer = renderer_from_string(override_find->second, this->backend);
    } else {
      this->renderer = renderer_from_string(find->second, this->backend);
    }
  } else {
    find = manifest.overrides.find("Gokai::Services::Compositor::renderer");
    if (find != manifest.overrides.end()) {
      this->renderer = renderer_from_string(find->second, this->backend);
    } else {
      this->renderer = wlr_renderer_autocreate(this->backend);
    }
  }

  this->allocator = wlr_allocator_autocreate(this->backend, this->renderer);

  wlr_renderer_init_wl_display(this->renderer, this->display);
  wlr_data_device_manager_create(this->display);

  this->logger->debug("Attaching Wayland Event loop to context event loop");
  auto event_loop = wl_display_get_event_loop(this->display);
  uv_poll_init(this->context->getLoop(), &this->event_poll, wl_event_loop_get_fd(event_loop));
  uv_poll_start(&this->event_poll, UV_READABLE | UV_WRITABLE, Compositor::poll_event_handle);

  this->socket = wl_display_add_socket_auto(this->display);
  setenv("WAYLAND_DISPLAY", this->socket, true);
  this->logger->info("Wayland is running on display \"{}\"", this->socket);
}

Compositor::~Compositor() {
  uv_poll_stop(&this->event_poll);
  wl_display_destroy(this->display);
}

void Compositor::start() {
  if (!wlr_backend_start(this->backend)) {
    throw std::runtime_error("Failed to start backend");
  }
}

struct wl_display* Compositor::getDisplay() {
  return this->display;
}

struct wlr_backend* Compositor::getBackend() {
  return this->backend;
}

struct wlr_renderer* Compositor::getRenderer() {
  return this->renderer;
}

struct wlr_allocator* Compositor::getAllocator() {
  return this->allocator;
}