#include <gokai/framework/os/linux/graphics/wayland/server/texture.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/view/wayland/server/window.h>
#include <gokai/services/engine-manager.h>
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

  this->new_subsurface_listener.notify = Window::new_subsurface_handler;
  wl_signal_add(&this->value->events.new_subsurface, &this->new_subsurface_listener);

  this->destroy_listener.notify = Window::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);

  this->onEnter.push_back([this]() {
    auto rect = this->getRect();
    auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
    auto output = wlr_output_layout_output_at(display_manager->getLayout(), rect.pos.x, rect.pos.y);
    if (output != nullptr) wlr_surface_send_enter(this->value, output);
  });

  this->onLeave.push_back([this]() {
    auto rect = this->getRect();
    auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
    auto output = wlr_output_layout_output_at(display_manager->getLayout(), rect.pos.x, rect.pos.y);
    if (output != nullptr) wlr_surface_send_leave(this->value, output);
  });

  struct wlr_subsurface* subsurface;
  wl_list_for_each(subsurface, &this->value->current.subsurfaces_below, current.link) {
    Window::new_subsurface_handler(&this->new_subsurface_listener, subsurface);
  }

  wl_list_for_each(subsurface, &this->value->current.subsurfaces_above, current.link) {
    Window::new_subsurface_handler(&this->new_subsurface_listener, subsurface);
  }

  this->value->data = this;
}

Window::~Window() {
  for (const auto& func : this->destroy) func();

  auto texture_manager = reinterpret_cast<Gokai::Services::TextureManager*>(this->context->getSystemService(Gokai::Services::TextureManager::SERVICE_NAME));
  if (this->texture_id > 0) {
    texture_manager->unregister(this->texture_id);
  }
}

bool Window::isToplevel() {
  return !wlr_surface_is_subsurface(this->value);
}

std::string Window::getRole() {
  if (wlr_surface_is_xdg_surface(this->value)) return "xdg-surface";
  else if (wlr_surface_is_subsurface(this->value)) return "subsurface";
  return "";
}

std::list<xg::Guid> Window::getChildrenAbove() {
  std::list<xg::Guid> list;
  struct wlr_subsurface* subsurface;

  wl_list_for_each(subsurface, &this->value->current.subsurfaces_above, current.link) {
    auto inst = static_cast<Window*>(subsurface->surface->data);
    list.push_back(inst->getId());
  }
  return list;
}

std::list<xg::Guid> Window::getChildrenBelow() {
  std::list<xg::Guid> list;
  struct wlr_subsurface* subsurface;

  wl_list_for_each(subsurface, &this->value->current.subsurfaces_below, current.link) {
    auto inst = static_cast<Window*>(subsurface->surface->data);
    list.push_back(inst->getId());
  }
  return list;
}

std::string Window::getDisplayName() {
  auto rect = this->getRect();
  auto display_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::DisplayManager*>(this->context->getSystemService(Gokai::Services::DisplayManager::SERVICE_NAME));
  auto output = wlr_output_layout_output_at(display_manager->getLayout(), rect.pos.x, rect.pos.y);
  if (output == nullptr) return nullptr;
  return output->name;
}

bool Window::hasDecorations() {
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdgDecoration(this->getId());
  if (xdg != nullptr) {
    return xdg->isClientSide();
  }
  return false;
}

bool Window::hasTexture() {
  return wlr_surface_has_buffer(this->value);
}

int64_t Window::getTextureId() {
  return this->texture_id;
}

std::shared_ptr<Gokai::Graphics::Texture> Window::getTexture() {
  if (wlr_surface_has_buffer(this->value)) {
    if (this->texture == nullptr) {
      this->texture.reset(new Gokai::Framework::os::Linux::Graphics::Wayland::Server::Texture(Gokai::ObjectArguments({
        { "value", &this->value->buffer->base },
      })));
      this->texture->onFrame.push_back([this]() {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        wlr_surface_send_frame_done(this->value, &now);
      });
    } else {
    }
    return this->texture;
  }

  this->texture.reset();
  return nullptr;
}

Gokai::View::URect Window::getRect() {
  auto rect = Gokai::View::Window::getRect();
  rect.size.x = this->value->current.width;
  rect.size.y = this->value->current.height;

  if (wlr_surface_is_subsurface(this->value)) {
    auto subsurface = wlr_subsurface_from_wlr_surface(this->value);
    rect.pos.x = subsurface->current.x;
    rect.pos.y = subsurface->current.y;
    return rect;
  }

  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdg(this->getId());
  if (xdg != nullptr) {
    struct wlr_box box;
    wlr_xdg_surface_get_geometry(xdg->getValue(), &box);

    rect.pos.x = box.x;
    rect.pos.y = box.y;
    rect.size.x = box.width;
    rect.size.y = box.height;
  }
  return rect;
}

void Window::setRect(Gokai::View::URect rect) {
  Gokai::View::Window::setRect(rect);

  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdg(this->getId());
  if (xdg != nullptr) {
    if (xdg->getValue()->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
      wlr_xdg_toplevel_set_size(xdg->getValue()->toplevel, rect.size.x, rect.size.y);
    } else if (xdg->getValue()->role == WLR_XDG_SURFACE_ROLE_POPUP) {
      struct wlr_box box = {
        .x = rect.pos.x,
        .y = rect.pos.y,
        .width = rect.size.x,
        .height = rect.size.y,
      };
      wlr_xdg_popup_unconstrain_from_box(xdg->getValue()->popup, &box);
    }
  }
}

std::string Window::getTitle() {
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdg(this->getId());
  if (xdg != nullptr) {
    if (xdg->getValue()->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
      return xdg->getValue()->toplevel->title;
    }
  }
  return "";
}

bool Window::isActive() {
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdg(this->getId());
  if (xdg != nullptr) {
    if (xdg->getValue()->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
      return xdg->getValue()->toplevel->current.activated;
    }
  }
  return Gokai::View::Window::isActive();
}

void Window::setActive(bool value) {
  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));
  auto xdg = window_manager->getXdg(this->getId());
  if (xdg != nullptr) {
    if (xdg->getValue()->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
      auto input_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::InputManager*>(this->context->getSystemService(Gokai::Services::InputManager::SERVICE_NAME));
      auto seat = input_manager->getSeat();
      auto prev_surface = seat->keyboard_state.focused_surface == nullptr ? seat->pointer_state.focused_surface : seat->keyboard_state.focused_surface;

      if (prev_surface != nullptr && prev_surface != this->value && value) {
        auto prev_win = reinterpret_cast<Window*>(prev_surface->data);
        prev_win->setActive(false);
      }

      wlr_xdg_toplevel_set_activated(xdg->getValue()->toplevel, value);
      if (value) {
        auto kb = wlr_seat_get_keyboard(seat);
        if (kb != nullptr) {
          wlr_seat_keyboard_enter(seat, this->value, kb->keycodes, kb->num_keycodes, &kb->modifiers);
        }

        wlr_seat_pointer_notify_enter(seat, this->value, 0, 0);
      }

      for (const auto& func : this->onActive) func();
      return;
    }
  }

  Gokai::View::Window::setActive(value);
  for (const auto& func : this->onActive) func();
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
  } else if (self->hasTexture() && self->texture_id > 0) {
    static_cast<Gokai::Framework::os::Linux::Graphics::Wayland::Server::Texture*>(self->texture.get())->commit(&self->value->buffer->base);
  }

  for (const auto& func : self->onCommit) func();
}

void Window::new_subsurface_handler(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, new_subsurface_listener);
  auto subsurface = reinterpret_cast<struct wlr_subsurface*>(data);

  auto window_manager = reinterpret_cast<Gokai::Framework::os::Linux::Services::Wayland::Server::WindowManager*>(self->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));

  auto id = xg::newGuid();
  window_manager->logger->debug("New subsurface {}", id.str());
  auto window = new Gokai::Framework::os::Linux::View::Wayland::Server::Window(Gokai::ObjectArguments({
    { "logger", self->getLogger() },
    { "id", id },
    { "context", self->context },
    { "value", subsurface->surface },
  }));

  window->onMapped.push_back([self, window_manager, id]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(self->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "mapped";
    call.arguments = id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", window_manager->method_codec.encodeMethodCall(call));
  });

  window->onCommit.push_back([self, window_manager, id]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(self->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "commit";
    call.arguments = id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", window_manager->method_codec.encodeMethodCall(call));
  });

  window->destroy.push_back([window_manager, id]() {
    window_manager->windows.erase(id);
    for (const auto& func : window_manager->changed) func();
  });

  window_manager->windows[id] = window;
  for (const auto& func : window_manager->changed) func();
}

void Window::破壊する(struct wl_listener* listener, void* data) {
  Window* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}