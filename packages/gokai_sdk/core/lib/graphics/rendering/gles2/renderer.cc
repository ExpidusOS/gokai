#include <gokai/graphics/rendering/gles2/renderer.h>

using namespace Gokai::Graphics::Rendering::GLES2;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {
  this->config = {};
  this->config.type = kOpenGL;
  this->config.open_gl.struct_size = sizeof (FlutterRendererConfig);
  this->config.open_gl.fbo_reset_after_present = true;
  this->config.open_gl.present_with_info = Renderer::present_with_info_callback;
  this->config.open_gl.fbo_callback = Renderer::fbo_callback;
}

FlutterRendererConfig* Renderer::getConfig() {
  return &this->config;
}

void Renderer::init() {}

bool Renderer::present_with_info_callback(void* data, const FlutterPresentInfo* info) {
  return true;
}

uint32_t Renderer::fbo_callback(void* data) {
  return 0;
}