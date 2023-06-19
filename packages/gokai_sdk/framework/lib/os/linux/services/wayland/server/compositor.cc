#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <stdio.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

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

Compositor::Compositor(Gokai::ObjectArguments arguments) : Gokai::Services::Compositor(arguments) {
  wlr_log_init(spdlog2wlr(this->logger->level()), spdlog4wlr);

  this->display = wl_display_create();

  this->backend = wlr_backend_autocreate(this->display);
  if (this->backend == nullptr) {
    throw std::runtime_error("Failed to create wlroots backend");
  }
}

Compositor::~Compositor() {
  wl_display_destroy(this->display);
}