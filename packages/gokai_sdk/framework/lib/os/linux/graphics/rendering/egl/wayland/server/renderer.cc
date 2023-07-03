#include <gokai/flutter/engine.h>
#include <gokai/framework/os/linux/graphics/rendering/egl/wayland/server/renderer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/services/texture-manager.h>

using namespace Gokai::Framework::os::Linux::Graphics::Rendering::EGL::Wayland::Server;

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

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments), value{std::any_cast<struct wlr_renderer*>(arguments.get("value"))} {
  this->gk_context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(this->gk_context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  auto egl = wlr_gles2_renderer_get_egl(this->value);
  auto display = wlr_egl_get_display(egl);
  auto context = wlr_egl_get_context(egl);

  EGLConfig egl_config;
  EGLint matched = 0;
  if (!eglChooseConfig(display, config_attribs, &egl_config, 1, &matched)) {
    throw std::runtime_error("eglChooseConfig() has failed");
  }

  if (matched == 0) {
    throw std::runtime_error("eglChooseConfig() returned no matches");
  }

  this->logger->debug("EGL config {} was chosen", egl_config);
  this->context = eglCreateContext(display, egl_config, context, context_attribs);
  if (this->context == EGL_NO_CONTEXT) {
    throw std::runtime_error("Failed to create an EGL context");
  }

  this->resource_context = eglCreateContext(display, egl_config, context, context_attribs);
  if (this->resource_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("Failed to create an EGL context");
  }

  this->buffer = new Gokai::Framework::os::Linux::View::Wayland::Server::EGL::Buffer(Gokai::ObjectArguments({
    { "allocator", compositor->getAllocator() },
    { "renderer", this->value },
    { "logger", arguments.get("logger") },
    { "format", arguments.get("format") },
    { "size", arguments.get("size") },
  }));

  this->config = {};
  this->config.type = kOpenGL;
  this->config.open_gl.struct_size = sizeof (FlutterRendererConfig);
  this->config.open_gl.fbo_reset_after_present = true;
  this->config.open_gl.fbo_with_frame_info_callback = Renderer::fbo_with_frame_info_callback;
  this->config.open_gl.make_current = Renderer::make_current_callback;
  this->config.open_gl.clear_current = Renderer::clear_current_callback;
  this->config.open_gl.make_resource_current = Renderer::make_resource_current_callback;
  this->config.open_gl.present_with_info = Renderer::present_with_info_callback;
  this->config.open_gl.gl_external_texture_frame_callback = Renderer::gl_external_texture_frame_callback;
}

Renderer::~Renderer() {
  delete this->buffer;
}

FlutterRendererConfig* Renderer::getConfig() {
  return &this->config;
}

void Renderer::resize(glm::uvec2 size) {
  struct wlr_dmabuf_attributes attribs;
  wlr_buffer_get_dmabuf(this->buffer->getBuffer(), &attribs);

  auto compositor = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::Compositor*>(this->gk_context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
  auto new_buffer = new Gokai::Framework::os::Linux::View::Wayland::Server::EGL::Buffer(Gokai::ObjectArguments({
    { "allocator", compositor->getAllocator() },
    { "renderer", this->value },
    { "logger", this->getLogger() },
    { "format", attribs.format },
    { "size", size },
  }));

  delete this->buffer;
  this->buffer = new_buffer;
}

bool Renderer::make_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return false;
  auto self = static_cast<Renderer*>(engine->getRenderer());
  auto egl = wlr_gles2_renderer_get_egl(self->value);
  auto display = wlr_egl_get_display(egl);

  return eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, self->context);
}

bool Renderer::clear_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return false;

  auto self = static_cast<Renderer*>(engine->getRenderer());
  auto egl = wlr_gles2_renderer_get_egl(self->value);
  auto display = wlr_egl_get_display(egl);

  return eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

bool Renderer::make_resource_current_callback(void* data) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return false;

  auto self = static_cast<Renderer*>(engine->getRenderer());
  auto egl = wlr_gles2_renderer_get_egl(self->value);
  auto display = wlr_egl_get_display(egl);

  return eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, self->resource_context);
}

uint32_t Renderer::fbo_with_frame_info_callback(void* data, const FlutterFrameInfo* info) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return 0;

  auto self = static_cast<Renderer*>(engine->getRenderer());
  return self->buffer->getFbo();
}

bool Renderer::present_with_info_callback(void* data, const FlutterPresentInfo* info) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return false;

  auto self = static_cast<Renderer*>(engine->getRenderer());

  for (size_t i = 0; i < info->frame_damage.num_rects; i++) {
    auto damage = info->frame_damage.damage[i];

    self->damage.push_back(Gokai::View::DRect(
      glm::vec2(damage.left, damage.top),
      glm::vec2(damage.right - damage.left, damage.bottom - damage.top)
    ));
  }
  return true;
}

bool Renderer::gl_external_texture_frame_callback(void* data, int64_t tid, size_t width, size_t height, FlutterOpenGLTexture* out) {
  auto engine = static_cast<Gokai::Flutter::Engine*>(data);
  if (engine->isShutdown()) return false;

  auto self = static_cast<Renderer*>(engine->getRenderer());

  auto texture_manager = reinterpret_cast<Gokai::Services::TextureManager*>(self->gk_context->getSystemService(Gokai::Services::TextureManager::SERVICE_NAME));
  auto texture = texture_manager->get(tid);
  if (texture == nullptr) return false;

  Gokai::Flutter::Texture tx_out;
  auto result = texture->frame(width, height, &tx_out);
  *out = tx_out.open_gl;
  return result;
}