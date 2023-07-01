#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering/egl/renderer.h>

using namespace Gokai::Graphics::Rendering::EGL;

static const EGLint config_attribs[] = {
  EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
  EGL_RED_SIZE, 8,
  EGL_GREEN_SIZE, 8,
  EGL_BLUE_SIZE, 8,
  EGL_ALPHA_SIZE, 8,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  EGL_NONE,
};

static const EGLint context_attribs[] = {
  EGL_CONTEXT_MAJOR_VERSION, 3,
  EGL_CONTEXT_MAJOR_VERSION, 2,
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

  this->size = std::any_cast<glm::uvec2>(arguments.get("size"));

  EGLint surface_attribs[] = {
    EGL_WIDTH, this->size.x,
    EGL_HEIGHT, this->size.y,
    EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
    EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
    EGL_NONE,
  };

  this->surface = eglCreatePbufferSurface(this->display, egl_config, surface_attribs);
  if (this->surface == EGL_NO_SURFACE) {
    throw std::runtime_error("Failed to create an EGL surface");
  }

  EGLint render_buffer;
  eglQuerySurface(this->display, this->surface, EGL_RENDER_BUFFER, &render_buffer);
  this->logger->debug("Using render buffer {} on surface", render_buffer);

  EGLint texture;
  eglQuerySurface(this->display, this->surface, EGL_TEXTURE_TARGET, &texture);
  if (texture != EGL_NO_TEXTURE) this->logger->debug("Using texture {} on surface", texture);

  auto config = this->getConfig();
  config->open_gl.make_current = Renderer::make_current_callback;
  config->open_gl.clear_current = Renderer::clear_current_callback;
  config->open_gl.make_resource_current = Renderer::make_resource_current_callback;
  config->open_gl.present_with_info = Renderer::present_with_info_callback;
}

Renderer::~Renderer() {
  eglDestroySurface(this->display, this->surface);
  eglDestroyContext(this->display, this->resource_context);
  eglDestroyContext(this->display, this->context);
}

bool Renderer::make_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());
  return eglMakeCurrent(self->display, self->surface, self->surface, self->context);
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

bool Renderer::present_with_info_callback(void* data, const FlutterPresentInfo* info) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  auto self = static_cast<Renderer*>(engine->getRenderer());

  return eglSwapBuffers(self->display, self->surface) == EGL_TRUE;
}