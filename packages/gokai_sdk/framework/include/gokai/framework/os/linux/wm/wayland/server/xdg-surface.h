#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/logging.h>
#include <functional>
#include <list>
#include <wayland-server-core.h>

extern "C" {
#define static
#include <wlr/types/wlr_xdg_shell.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace WM {
          namespace Wayland {
            namespace Server {
              class XdgSurface : public Gokai::Loggable {
                public:
                  XdgSurface(Gokai::ObjectArguments arguments);
                  ~XdgSurface();

                  std::list<std::function<void()>> destroy;

                  xg::Guid getId();
                  struct wlr_xdg_surface* getValue();
                private:
                  struct wl_listener destroy_listener;

                  xg::Guid id;
                  struct wlr_xdg_surface* value;

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