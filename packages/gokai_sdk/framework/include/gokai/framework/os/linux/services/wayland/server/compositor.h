#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor.h>

extern "C" {
#define static
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/util/log.h>
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
              class Compositor : public Gokai::Services::Compositor {
                public:
                  Compositor(Gokai::ObjectArguments arguments);
                  ~Compositor();

                  void start();
                  struct wlr_backend* getBackend();
                private:
                  static void poll_event_handle(uv_poll_t* event_poll, int status, int events);

                  struct wl_display* display;
                  struct wlr_backend* backend;
                  struct wlr_renderer* renderer;
                  struct wlr_allocator* allocator;
                  uv_poll_t event_poll;
              };
            }
          }
        }
      }
    }
  }
}
#endif