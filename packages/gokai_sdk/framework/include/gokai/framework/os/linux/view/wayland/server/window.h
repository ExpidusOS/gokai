#pragma once

#ifdef __cplusplus
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

                  std::string getDisplayName() override;

                  std::list<std::function<void()>> destroy;
                private:
                  std::shared_ptr<Gokai::Context> context;
                  struct wlr_surface* value;
                  struct wl_listener commit_listener;
                  struct wl_listener destroy_listener;

                  static void commit_handler(struct wl_listener* listener, void* data);
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