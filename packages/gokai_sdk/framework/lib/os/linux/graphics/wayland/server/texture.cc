#include <gokai/framework/os/linux/graphics/wayland/server/texture.h>
#include <GL/gl.h>

using namespace Gokai::Framework::os::Linux::Graphics::Wayland::Server;

Texture::Texture(Gokai::ObjectArguments arguments) : Gokai::Graphics::Texture(arguments), buffer{std::any_cast<struct wlr_buffer*>(arguments.get("buffer"))}, value{std::any_cast<struct wlr_texture*>(arguments.get("value"))} {
}

struct wlr_buffer* Texture::getBuffer() {
  return this->buffer;
}

struct wlr_texture* Texture::getValue() {
  return this->value;
}

bool Texture::frame(Gokai::Flutter::Engine* engine, size_t width, size_t height, Gokai::Flutter::Texture* out) {
  if (wlr_texture_is_gles2(this->value)) {
    struct wlr_gles2_texture_attribs attribs;
    wlr_gles2_texture_get_attribs(this->value, &attribs);

    out->open_gl = {
      .target = attribs.target,
      .name = attribs.tex,
      .format = GL_RGBA8,
      .user_data = this,
      .destruction_callback = [](void* user_data) {
      },
      .width = this->value->width,
      .height = this->value->height,
    };
    return true;
  }
  return false;
}