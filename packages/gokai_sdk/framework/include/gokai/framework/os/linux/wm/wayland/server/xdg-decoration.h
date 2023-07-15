#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/logging.h>
#include <functional>
#include <list>
#include <wayland-server-core.h>

extern "C" {
#define static
#include <wlr/types/wlr_xdg_decoration_v1.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace WM {
          namespace Wayland {
            namespace Server {
              class XdgDecoration : public Gokai::Loggable {
                public:
                  XdgDecoration(Gokai::ObjectArguments arguments);
                  ~XdgDecoration();

                  std::list<std::function<void()>> destroy;

                  bool isClientSide();

                  xg::Guid getId();
                  struct wlr_xdg_toplevel_decoration_v1* getValue();
                private:
                  struct wl_listener destroy_listener;
                  struct wl_listener mode_listener;

                  bool is_client_side;

                  xg::Guid id;
                  struct wlr_xdg_toplevel_decoration_v1* value;

                  static void 破壊する(struct wl_listener* listener, void* data);
                  static void mode_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif