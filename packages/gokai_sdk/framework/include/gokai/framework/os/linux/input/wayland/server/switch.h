#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/switch.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_switch.h>
#include <wayland-server.h>
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Input {
          namespace Wayland {
            namespace Server {
              class Switch : public Base, public Gokai::Input::Switch {
                public:
                  Switch(Gokai::ObjectArguments arguments);

                  struct wlr_switch* getSwitchValue();
                private:
                  struct wl_listener toggle_listener;

                  static void toggle_handler(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif
