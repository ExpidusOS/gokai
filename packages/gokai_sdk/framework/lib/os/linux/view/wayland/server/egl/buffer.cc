#include <gokai/framework/os/linux/view/wayland/server/egl/buffer.h>

#define TAG "Gokai::Framework::os::Linux::View::Wayland::Server::EGL::Buffer"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server::EGL;

Buffer::Buffer(Gokai::ObjectArguments arguments) : Gokai::View::Displayable(arguments), Gokai::Loggable(TAG, arguments) {
  auto allocator = std::any_cast<struct wlr_allocator*>(arguments.get("allocator"));
  auto size = std::any_cast<glm::uvec2>(arguments.get("size"));
  this->renderer = std::any_cast<struct wlr_renderer*>(arguments.get("renderer"));

  auto formats = wlr_renderer_get_dmabuf_texture_formats(this->renderer);
  auto format = wlr_drm_format_set_get(formats, std::any_cast<uint32_t>(arguments.get("format")));

  this->buffer = wlr_allocator_create_buffer(allocator, size.x, size.y, format);
  this->texture = wlr_texture_from_buffer(this->renderer, this->buffer);

  auto old_ctx = eglGetCurrentContext();
  auto old_draw_surf = eglGetCurrentSurface(EGL_DRAW);
  auto old_read_surf = eglGetCurrentSurface(EGL_READ);

  struct wlr_gles2_texture_attribs attribs;
  wlr_gles2_texture_get_attribs(this->texture, &attribs);

  auto egl = wlr_gles2_renderer_get_egl(this->renderer);
  eglMakeCurrent(wlr_egl_get_display(egl), EGL_NO_SURFACE, EGL_NO_SURFACE, wlr_egl_get_context(egl));

  glGenFramebuffers(1, &this->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, attribs.target, attribs.tex, 0);
  GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  eglMakeCurrent(wlr_egl_get_display(egl), old_draw_surf, old_read_surf, old_ctx);

  if (fb_status != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Failed to create fbo");
  }
}

Buffer::~Buffer() {
  auto old_ctx = eglGetCurrentContext();
  auto old_draw_surf = eglGetCurrentSurface(EGL_DRAW);
  auto old_read_surf = eglGetCurrentSurface(EGL_READ);

  auto egl = wlr_gles2_renderer_get_egl(this->renderer);
  eglMakeCurrent(wlr_egl_get_display(egl), EGL_NO_SURFACE, EGL_NO_SURFACE, wlr_egl_get_context(egl));
  glDeleteFramebuffers(1, &this->fbo);
  eglMakeCurrent(wlr_egl_get_display(egl), old_draw_surf, old_read_surf, old_ctx);

  wlr_texture_destroy(this->texture);
  wlr_buffer_drop(this->buffer);
}

Gokai::View::URect Buffer::getRect() {
  return Gokai::View::URect(
    glm::uvec2(),
    glm::uvec2(this->texture->width, this->texture->height)
  );
}

struct wlr_texture* Buffer::getTexture() {
  return this->texture;
}

struct wlr_buffer* Buffer::getBuffer() {
  return this->buffer;
}

GLuint Buffer::getFbo() {
  return this->fbo;
}