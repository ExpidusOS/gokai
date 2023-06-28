#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <list>
#include <functional>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_input_device.h>
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
              class Base : public Gokai::Input::Base {
                public:
                  Base(Gokai::ObjectArguments arguments);
                  ~Base();

                  struct wlr_input_device* getValue();
                  std::string getName() override;

                  std::list<std::function<void()>> destroy;
                protected:
                  int32_t id;
                private:
                  struct wlr_input_device* value;
                  struct wl_listener destroy_listener;

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