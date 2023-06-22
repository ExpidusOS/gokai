#pragma once

#ifdef __cplusplus
#include <gokai/graphics/rendering.h>
#include <gokai/view/display.h>
#include <gokai/context.h>
#include <gokai/logging.h>

extern "C" {
#define static
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/render/pixman.h>
#include <wlr/render/vulkan.h>
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
                  Gokai::Graphics::Renderer* renderer;
                  Gokai::Context* context;
                  struct wlr_output* value;

                  struct wl_listener frame_listener;
                  struct wl_listener destroy_listener;

                  static void フレーム(struct wl_listener* listener, void* data);
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