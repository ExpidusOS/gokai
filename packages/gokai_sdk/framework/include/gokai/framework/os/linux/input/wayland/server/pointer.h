#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/pointer.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_pointer.h>
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
              class Pointer : public Base, public Gokai::Input::Pointer {
                public:
                  Pointer(Gokai::ObjectArguments arguments);

                  struct wlr_pointer* getPointerValue();
              };
            }
          }
        }
      }
    }
  }
}
#endif