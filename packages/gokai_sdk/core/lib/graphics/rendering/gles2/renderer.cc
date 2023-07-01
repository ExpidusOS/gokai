#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering/gles2/renderer.h>

using namespace Gokai::Graphics::Rendering::GLES2;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {
  this->config = {};
  this->config.type = kOpenGL;
  this->config.open_gl.struct_size = sizeof (FlutterRendererConfig);
  this->config.open_gl.fbo_reset_after_present = true;
  this->config.open_gl.fbo_with_frame_info_callback = Renderer::fbo_with_frame_info_callback;
}

FlutterRendererConfig* Renderer::getConfig() {
  return &this->config;
}

uint32_t Renderer::fbo_with_frame_info_callback(void* data, const FlutterFrameInfo* info) {
  return 0;
}