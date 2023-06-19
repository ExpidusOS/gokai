#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor.h>

extern "C" {
#include <wlr/backend.h>
#include <wlr/util/log.h>
#include <wayland-server.h>
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class Compositor : public Gokai::Services::Compositor {
                public:
                  Compositor(Gokai::ObjectArguments arguments);
                  ~Compositor();
                private:
                  struct wl_display* display;
                  struct wlr_backend* backend;
              };
            }
          }
        }
      }
    }
  }
}
#endif