#pragma once

#ifdef __cplusplus
#include <gokai/services/display-manager.h>

extern "C" {
#include <wayland-server.h>
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class DisplayManager : public Gokai::Services::DisplayManager {
                public:
                  DisplayManager(Gokai::ObjectArguments arguments);
                private:
                  static void handle_display_new(struct wl_listener* listener, void* data);

                  struct wl_listener display_new;
              };
            }
          }
        }
      }
    }
  }
}
#endif