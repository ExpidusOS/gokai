#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor-input-method.h>

extern "C" {
#define static
#define delete _delete
#include <wlr/types/wlr_input_method_v2.h>
#include <wayland-server.h>
#undef delete
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class CompositorInputMethod : public Gokai::Services::CompositorInputMethod {
                public:
                  CompositorInputMethod(Gokai::ObjectArguments arguments);
                private:
                  struct wlr_input_method_manager_v2* manager;
              };
            }
          }
        }
      }
    }
  }
}
#endif