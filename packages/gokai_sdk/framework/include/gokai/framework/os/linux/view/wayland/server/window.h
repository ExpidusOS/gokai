#pragma once

#ifdef __cplusplus
#include <gokai/graphics/texture.h>
#include <gokai/view/window.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <functional>
#include <list>

extern "C" {
#define static
#include <wlr/types/wlr_compositor.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace View {
          namespace Wayland {
            namespace Server {
              class Window : public Gokai::View::Window, public Gokai::Loggable {
                public:
                  Window(Gokai::ObjectArguments arguments);
                  ~Window();

                  struct wlr_surface* getValue();

                  bool isToplevel() override;
                  std::string getRole() override;
                  std::list<xg::Guid> getChildrenAbove() override;
                  std::list<xg::Guid> getChildrenBelow() override;
                  std::string getDisplayName() override;
                  bool hasDecorations() override;
                  bool hasTexture() override;
                  int64_t getTextureId() override;
                  std::shared_ptr<Gokai::Graphics::Texture> getTexture() override;

                  Gokai::View::URect getRect() override;
                  void setRect(Gokai::View::URect rect) override;
                  bool isActive() override;
                  void setActive(bool value) override;

                  std::string getTitle() override;

                  std::list<std::function<void()>> destroy;
                private:
                  struct wlr_surface* value;
                  struct wl_listener commit_listener;
                  struct wl_listener new_subsurface_listener;
                  struct wl_listener destroy_listener;
                  int64_t texture_id;
                  std::shared_ptr<Gokai::Graphics::Texture> texture;

                  static void commit_handler(struct wl_listener* listener, void* data);
                  static void new_subsurface_handler(struct wl_listener* listener, void* data);
                  static void 破壊する(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif
