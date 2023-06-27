#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/keyboard.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_keyboard.h>
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
              class Keyboard : public Base, public Gokai::Input::Keyboard {
                public:
                  Keyboard(Gokai::ObjectArguments arguments);

                  struct wlr_keyboard* getKeyboardValue();
              };
            }
          }
        }
      }
    }
  }
}
#endif