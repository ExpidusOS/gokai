#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/view/wayland/server/display.h>
#include <gokai/services/display-manager.h>

extern "C" {
#define static
#include <wlr/types/wlr_output_layout.h>
#include <wayland-server.h>
#undef static
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

                  struct wlr_output_layout* getLayout();
                private:
                  static void handle_display_new(struct wl_listener* listener, void* data);

                  struct wl_listener display_new;
                  std::list<Gokai::Framework::os::Linux::View::Wayland::Server::Display*> displays;
                  struct wlr_output_layout* layout;
              };
            }
          }
        }
      }
    }
  }
}
#endif