#pragma once

#ifdef __cplusplus
#include <gokai/view/window.h>
#include <gokai/logging.h>
#include <functional>
#include <list>

extern "C" {
#define static
#include <wlr/types/wlr_compositor.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace View {
          namespace Wayland {
            namespace Server {
              class Window : public Gokai::View::Window, public Gokai::Loggable {
                public:
                  Window(Gokai::ObjectArguments arguments);
                  ~Window();

                  std::list<std::function<void()>> destroy;
                private:
                  struct wlr_surface* value;
              };
            }
          }
        }
      }
    }
  }
}
#endif