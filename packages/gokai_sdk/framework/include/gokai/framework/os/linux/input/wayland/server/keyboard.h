#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/keyboard.h>

extern "C" {
#include <wayland-server-core.h>
#include <xkbcommon/xkbcommon.h>
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
                  ~Keyboard();

                  struct wlr_keyboard* getKeyboardValue();
                private:
                  struct xkb_context* context;
                  struct xkb_keymap* keymap;
                  struct wlr_seat* seat;
                  struct wl_listener key_listener;
                  struct wl_listener modifiers_listener;

                  static void key_handle(struct wl_listener* listener, void* data);
                  static void modifiers_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif