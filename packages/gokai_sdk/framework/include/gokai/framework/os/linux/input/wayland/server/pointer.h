#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/pointer.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_cursor.h>
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
                  ~Pointer();

                  struct wlr_pointer* getPointerValue();
                  struct wlr_cursor* getCursor();
                private:
                  struct wlr_cursor* cursor;
              };
            }
          }
        }
      }
    }
  }
}
#endif