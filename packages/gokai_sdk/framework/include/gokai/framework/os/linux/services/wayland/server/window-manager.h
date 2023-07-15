#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/wm/wayland/server/xdg-decoration.h>
#include <gokai/framework/os/linux/wm/wayland/server/xdg-surface.h>
#include <gokai/services/window-manager.h>

extern "C" {
#define static
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
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

                  std::map<xg::Guid, Gokai::View::Window*> windows;

                  std::list<xg::Guid> getIds() override;
                  Gokai::View::Window* get(xg::Guid id) override;

                  Gokai::Framework::os::Linux::WM::Wayland::Server::XdgDecoration* getXdgDecoration(xg::Guid id);
                  Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface* getXdg(xg::Guid id);

                  struct wlr_compositor* getCompositor();
                  struct wlr_subcompositor* getSubcompositor();
                  struct wlr_xdg_decoration_manager_v1* getDecorationManager();
                  struct wlr_xdg_shell* getXdgShell();
                private:
                  struct wlr_compositor* compositor;
                  struct wlr_subcompositor* subcompositor;
                  struct wlr_xdg_shell* xdg_shell;
                  struct wlr_xdg_decoration_manager_v1* decoration_manager;
                  struct wl_listener new_surface_listener;
                  struct wl_listener new_xdg_surface_listener;
                  struct wl_listener new_toplevel_decoration_listener;

                  std::map<xg::Guid, Gokai::Framework::os::Linux::WM::Wayland::Server::XdgDecoration*> xdg_decorations;
                  std::map<xg::Guid, Gokai::Framework::os::Linux::WM::Wayland::Server::XdgSurface*> xdg_surfaces;

                  static void new_surface_handle(struct wl_listener* listener, void* data);
                  static void new_xdg_surface_handle(struct wl_listener* listener, void* data);
                  static void new_toplevel_decoration_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif