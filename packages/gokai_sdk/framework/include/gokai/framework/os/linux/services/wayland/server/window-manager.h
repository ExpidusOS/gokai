#pragma once

#ifdef __cplusplus
#include <gokai/services/window-manager.h>

extern "C" {
#define static
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xdg_shell.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class WindowManager : public Gokai::Services::WindowManager {
                public:
                  WindowManager(Gokai::ObjectArguments arguments);

                  std::list<xg::Guid> getIds() override;
                  Gokai::View::Window* get(xg::Guid id) override;

                  struct wlr_compositor* getCompositor();
                  struct wlr_subcompositor* getSubcompositor();
                  struct wlr_xdg_shell* getXdgShell();
                private:
                  struct wlr_compositor* compositor;
                  struct wlr_subcompositor* subcompositor;
                  struct wlr_xdg_shell* xdg_shell;
                  struct wl_listener new_surface_listener;

                  std::map<xg::Guid, Gokai::View::Window*> windows;

                  static void new_surface_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif