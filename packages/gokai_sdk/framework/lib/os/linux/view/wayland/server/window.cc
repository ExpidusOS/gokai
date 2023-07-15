#include <gokai/framework/os/linux/graphics/wayland/server/texture.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>
#include <gokai/services/texture-manager.h>
#include <linux/dma-buf.h>
#include <sys/ioctl.h>
#include <time.h>
#include <xf86drm.h>

#define TAG "Gokai::View::Window"

using namespace Gokai::Framework::os::Linux::View::Wayland::Server;

struct CommitEvent {
  Window* window;
  uv_poll_t handle;
};

Window::Window(Gokai::ObjectArguments arguments) : Gokai::View::Window(arguments), Loggable(TAG, arguments), value{std::any_cast<struct wlr_surface*>(arguments.get("value"))}, texture_id{0} {
  this->context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));

  auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
  auto pos = input_manager->getActivePoint();

  auto rect = this->getRect();
  rect.pos = pos;
  this->setRect(rect);

  this->commit_listener.notify = Window::commit_handler;
  wl_signal_add(&this->value->events.commit, &this->commit_listener);

  this->destroy_listener.notify = Window::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Window::~Window() {
  for (const auto& func : this->destroy) func();

  auto texture_manager = reinterpret_cast<Gokai::Services::TextureManager*>(this->context->getSystemService(Gokai::Services::TextureManager::SERVICE_NAME));
  if (this->texture_id > 0) {
    texture_manager->unregister(this->texture_id);
  }
}

std::string Window::getDisplayName() {
  auto rect = this->getRect();
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  auto output = wlr_output_layout_output_at(display_manager->getLayout(), rect.pos.x, rect.pos.y);
  if (output == nullptr) return nullptr;
  return output->name;
}

bool Window::hasTexture() {
  return wlr_surface_has_buffer(this->value);
}

int64_t Window::getTextureId() {
  return this->texture_id;
}

std::shared_ptr<Gokai::Graphics::Texture> Window::getTexture() {
  if (wlr_surface_has_buffer(this->value)) {
    this->texture.reset(std::move(new Gokai::Framework::os::Linux::Graphics::Wayland::Server::Texture(Gokai::ObjectArguments({
      { "value", wlr_surface_get_texture(this->value) },
      { "buffer", &this->value->buffer->base },
    }))));
    this->texture->onFrame.push_back([this]() {
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      wlr_surface_send_frame_done(this->value, &now);
    });
    return this->texture;
  }

  this->texture.reset();
  return nullptr;
}

void Window::commit_handler(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, commit_listener);

  auto texture_manager = reinterpret_cast<Gokai::Services::TextureManager*>(self->context->getSystemService(Gokai::Services::TextureManager::SERVICE_NAME));

  if (self->hasTexture() && self->texture_id == 0) {
    struct wlr_dmabuf_attributes attribs;
    if (wlr_buffer_get_dmabuf(&self->value->buffer->base, &attribs)) {
      if (attribs.n_planes > 0) {
        struct dma_buf_export_sync_file sync_file = {
          .flags = DMA_BUF_SYNC_READ,
          .fd = -1,
        };

        int ret = -1;
        for (size_t i = 0; i < 4; i++) {
          ret = drmIoctl(attribs.fd[i], DMA_BUF_IOCTL_EXPORT_SYNC_FILE, &sync_file);
          if (ret == 0) break;
        }

        if (ret == 0) {
          auto event = new CommitEvent();
          event->window = self;
          uv_poll_init(self->context->getLoop(), &event->handle, sync_file.fd);
          uv_poll_start(&event->handle, UV_READABLE, [](uv_poll_t* handle, int status, int events) {
            auto event = reinterpret_cast<CommitEvent*>((char*)(handle) - offsetof(CommitEvent, handle));
            auto texture_manager = reinterpret_cast<Gokai::Services::TextureManager*>(event->window->context->getSystemService(Gokai::Services::TextureManager::SERVICE_NAME));
            event->window->texture_id = texture_manager->allocate(event->window->getTexture());
            for (const auto& func : event->window->onCommit) func();
            uv_poll_stop(handle);
            delete event;
          });
          return;
        }
      }
    }

    self->texture_id = texture_manager->allocate(self->getTexture());
  } else if (!self->hasTexture() && self->texture_id > 0) {
    texture_manager->unregister(self->texture_id);
    self->texture_id = 0;
  }

  for (const auto& func : self->onCommit) func();
}

void Window::破壊する(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}