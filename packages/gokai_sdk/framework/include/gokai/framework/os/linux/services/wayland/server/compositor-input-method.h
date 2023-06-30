#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor-input-method.h>

extern "C" {
#define static
#define delete _delete
#include <wlr/types/wlr_input_method_v2.h>
#include <wlr/types/wlr_text_input_v3.h>
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

                  void showInput() override;
                  void hideInput() override;
                private:
                  struct wlr_text_input_manager_v3* text_input_manager;
                  struct wlr_input_method_manager_v2* im_manager;

                  struct wl_listener text_input_listener;
                  struct wlr_text_input_v3* text_input;

                  struct wl_listener input_method_listener;
                  struct wlr_input_method_v2* input_method;

                  static void text_input_handle(struct wl_listener* listener, void* data);
                  static void input_method_handle(struct wl_listener* listener, void* data);
              };
            }
          }
        }
      }
    }
  }
}
#endif