#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering/cairo/renderer.h>
#include <stdexcept>

using namespace Gokai::Graphics::Rendering::Cairo;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {
  this->surface = nullptr;
  this->resize(std::any_cast<glm::uvec2>(arguments.get("size")));

  this->config.type = kSoftware;
  this->config.software = {
    .struct_size = sizeof (FlutterSoftwareRendererConfig),
    .surface_present_callback = Renderer::present,
  };
}

FlutterRendererConfig* Renderer::getConfig() {
  return &this->config;
}

glm::uvec2 Renderer::getSize() {
  if (this->surface == nullptr) return glm::uvec2(0, 0);

  return glm::uvec2(
    cairo_image_surface_get_width(this->surface),
    cairo_image_surface_get_height(this->surface)
  );
}

void Renderer::resize(glm::uvec2 size) {
  this->mx.lock();

  auto surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.x, size.y);
  if (surface == nullptr) {
    this->mx.unlock();
    throw std::runtime_error("Failed to generate image");
  }

  if (this->surface != nullptr) cairo_surface_destroy(this->surface);
  this->surface = surface;

  this->mx.unlock();
}

void Renderer::render(Gokai::View::Displayable& target) {
  auto img = static_cast<Gokai::View::Cairo::Image&>(target).getValue();

  if (this->surface == nullptr) return;

  this->mx.lock();
  this->logger->debug("Rendering to displayable");
  auto cr = cairo_create(img);

  cairo_set_source_surface(cr, this->surface, 0, 0);
  cairo_paint(cr);

  cairo_destroy(cr);
  this->mx.unlock();
}

bool Renderer::present(void* data, const void* buffer, size_t width, size_t height) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());

  if (!self->mx.try_lock()) return false;

  try {
    auto source = reinterpret_cast<const uint8_t*>(buffer);
    auto size = glm::uvec2(width, height);

    auto dest_size = self->getSize();
    if (size != dest_size) {
      self->logger->warn("Forcing a resize from {}x{} to {}x{}", dest_size.x, dest_size.y, size.x, size.y);
      self->mx.unlock();
      self->resize(size);
      self->mx.lock();

      dest_size = self->getSize();
    }

    cairo_surface_flush(self->surface);
    auto dest = reinterpret_cast<uint32_t*>(cairo_image_surface_get_data(self->surface));

    self->logger->debug("Rendering {} to {}x{}", buffer, dest_size.x, dest_size.y);

    for (size_t y = 0; y < size.y; y++) {
      for (size_t x = 0; x < size.x; x++) {
        size_t source_pos = y * width + x * 4;
        size_t dest_pos = y * width + x;

        if (source_pos >= width * height) break;

        auto r = source[source_pos];
        auto g = source[source_pos + 1];
        auto b = source[source_pos + 2];
        auto a = source[source_pos + 3];

        dest[dest_pos] = (a << 24) | (b << 16) | (g << 8) | r;
      }
    }

    cairo_surface_mark_dirty(self->surface);
    self->mx.unlock();
  } catch (const std::exception& ex) {
    self->mx.unlock();
    self->logger->error("Failed to present: {}", ex.what());
    return false;
  }
  return true;
}