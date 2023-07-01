#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering/egl/renderer.h>

using namespace Gokai::Graphics::Rendering::EGL;

static const EGLint config_attribs[] = {
  EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
  EGL_RED_SIZE, 8,
  EGL_GREEN_SIZE, 8,
  EGL_BLUE_SIZE, 8,
  EGL_ALPHA_SIZE, 8,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  EGL_NONE,
};

static const EGLint context_attribs[] = {
  EGL_CONTEXT_CLIENT_VERSION, 2,
  EGL_CONTEXT_PRIORITY_LEVEL_IMG, EGL_CONTEXT_PRIORITY_HIGH_IMG,
  EGL_NONE,
};

Renderer::Renderer(EGLDisplay display, EGLContext context, Gokai::ObjectArguments arguments) : Gokai::Graphics::Rendering::GLES2::Renderer(arguments), display{display} {
  EGLConfig egl_config;
  EGLint matched = 0;
  if (!eglChooseConfig(this->display, config_attribs, &egl_config, 1, &matched)) {
    throw std::runtime_error("eglChooseConfig() has failed");
  }

  if (matched == 0) {
    throw std::runtime_error("eglChooseConfig() returned no matches");
  }

  this->logger->debug("EGL config {} was chosen", egl_config);
  this->context = eglCreateContext(this->display, egl_config, context, context_attribs);
  if (this->context == EGL_NO_CONTEXT) {
    throw std::runtime_error("Failed to create an EGL context");
  }

  this->resource_context = eglCreateContext(this->display, egl_config, context, context_attribs);
  if (this->resource_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("Failed to create an EGL context");
  }

  auto config = this->getConfig();
  config->open_gl.make_current = Renderer::make_current_callback;
  config->open_gl.clear_current = Renderer::clear_current_callback;
  config->open_gl.make_resource_current = Renderer::make_resource_current_callback;

  this->init();
}

Renderer::~Renderer() {
  eglDestroyContext(this->display, this->resource_context);
  eglDestroyContext(this->display, this->context);
}

bool Renderer::make_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());
  return eglMakeCurrent(self->display, EGL_NO_SURFACE, EGL_NO_SURFACE, self->context);
}

bool Renderer::clear_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());
  return eglMakeCurrent(self->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

bool Renderer::make_resource_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());
  return eglMakeCurrent(self->display, EGL_NO_SURFACE, EGL_NO_SURFACE, self->resource_context);
}