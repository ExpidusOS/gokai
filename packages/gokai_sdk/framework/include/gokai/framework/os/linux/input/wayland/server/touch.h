#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/touch.h>

extern "C" {
#include <wayland-server-core.h>
#define static
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

                  struct wlr_seat* getSeat() override;

                  struct wlr_touch* getTouchValue();
                private:
                  struct wlr_seat* seat;
              };
            }
          }
        }
      }
    }
  }
}
#endif