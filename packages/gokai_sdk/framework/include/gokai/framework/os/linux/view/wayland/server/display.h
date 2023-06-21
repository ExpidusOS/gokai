#pragma once

#ifdef __cplusplus
#include <gokai/view/display.h>
#include <gokai/context.h>
#include <gokai/logging.h>

extern "C" {
#define static
#include <wlr/types/wlr_output.h>
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace View {
          namespace Wayland {
            namespace Server {
              class Display : public Gokai::View::Display, public Gokai::Loggable {
                public:
                  Display(Gokai::ObjectArguments arguments);
                  ~Display();

                  std::list<std::function<void()>> destroy;
                private:
                  Gokai::Context* context;
                  struct wlr_output* value;

                  struct wl_listener frame_listener;
                  struct wl_listener destroy_listener;

                  static void frame_handle(struct wl_listener* listener, void* data);
                  static void destroy_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif