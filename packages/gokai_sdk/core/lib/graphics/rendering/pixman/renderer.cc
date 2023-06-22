#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering/pixman/renderer.h>
#include <stdexcept>

using namespace Gokai::Graphics::Rendering::Pixman;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {
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

void Renderer::resize(glm::uvec2 size) {
  this->mx.lock();

  auto buffer = new uint32_t[size.x * size.y * 4];
  auto img = pixman_image_create_bits(PIXMAN_r8g8b8a8, size.x, size.y, buffer, 4);

  if (img == nullptr) {
    this->mx.unlock();
    delete buffer;
    throw std::runtime_error("Failed to generate image");
  }

  if (this->buffer != nullptr) delete this->buffer;
  if (this->img != nullptr) pixman_image_unref(this->img);

  this->buffer = buffer;
  this->img = img;
  this->size = size;

  this->mx.unlock();
}

void Renderer::render(Gokai::View::Displayable& target) {
  auto img = static_cast<Gokai::View::Pixman::Image&>(target).getValue();

  this->mx.lock();
  pixman_blt(
    pixman_image_get_data(this->img),
    pixman_image_get_data(img),
    pixman_image_get_stride(this->img),
    pixman_image_get_stride(img),
    pixman_image_get_depth(this->img),
    pixman_image_get_depth(img),
    0, 0, 0, 0,
    this->size.x, this->size.y
  );
  this->mx.unlock();
}

bool Renderer::present(void* data, const void* buffer, size_t width, size_t height) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());

  self->mx.lock();
  pixman_blt(
    (uint32_t*)reinterpret_cast<const uint32_t*>(buffer), self->buffer,
    pixman_image_get_stride(self->img),
    pixman_image_get_stride(self->img),
    pixman_image_get_depth(self->img),
    pixman_image_get_depth(self->img),
    0, 0, 0, 0,
    width, height
  );
  self->mx.unlock();
  return true;
}