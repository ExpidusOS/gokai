#include <gokai/framework/os/linux/graphics/wayland/server/texture.h>
#include <GL/gl.h>

using namespace Gokai::Framework::os::Linux::Graphics::Wayland::Server;

Texture::Texture(Gokai::ObjectArguments arguments) : Gokai::Graphics::Texture(arguments) {
  this->commit(std::any_cast<struct wlr_buffer*>(arguments.get("value")));
}

void Texture::commit(struct wlr_buffer* buffer) {
  this->chain.commitBuffer(std::shared_ptr<struct wlr_buffer>(wlr_buffer_lock(buffer), [buffer](auto _) {
    wlr_buffer_unlock(buffer);
  }));
}

bool Texture::frame(Gokai::Flutter::Engine* engine, size_t width, size_t height, Gokai::Flutter::Texture* out) {
  auto client_buffer = wlr_client_buffer_get(this->chain.startRead());

  if (wlr_texture_is_gles2(client_buffer->texture)) {
    struct wlr_gles2_texture_attribs attribs;
    wlr_gles2_texture_get_attribs(client_buffer->texture, &attribs);

    out->open_gl = {
      .target = attribs.target,
      .name = attribs.tex,
      .format = GL_RGBA8,
      .user_data = this,
      .destruction_callback = [](void* user_data) {
        // FIXME: causes segfaults with release to the shared_ptr
        //auto self = reinterpret_cast<Texture*>(user_data);
        //self->chain.endRead();
      },
      .width = client_buffer->texture->width,
      .height = client_buffer->texture->height,
    };
    return true;
  }
  return false;
}