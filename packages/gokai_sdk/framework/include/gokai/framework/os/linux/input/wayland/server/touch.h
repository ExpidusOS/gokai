#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/touch.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_touch.h>
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
              class Touch : public Base, public Gokai::Input::Touch {
                public:
                  Touch(Gokai::ObjectArguments arguments);
                  ~Touch();

                  struct wlr_touch* getTouchValue();
                private:
                  struct wl_listener touch_down_listener;
                  struct wl_listener touch_up_listener;
                  struct wl_listener touch_motion_listener;
                  struct wl_listener touch_cancel_listener;

                  std::unordered_map<int32_t, std::pair<double, double>> last_touch_coords;

                  static void touch_down_handler(struct wl_listener* listener, void* data);
                  static void touch_up_handler(struct wl_listener* listener, void* data);
                  static void touch_motion_handler(struct wl_listener* listener, void* data);
                  static void touch_cancel_handler(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif
