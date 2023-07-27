#pragma once

#ifdef __cplusplus
#include <gokai/flutter/engine.h>
#include <gokai/graphics/rendering.h>
#include <gokai/view/display.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <memory>

extern "C" {
#define static
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/render/pixman.h>
#include <wlr/render/vulkan.h>
#include <wlr/types/wlr_matrix.h>
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

                  std::string getName() override;
                  std::string getModel() override;
                  bool isHDR() override;
                  glm::uvec2 getPhysicalSize() override;
                  Gokai::View::DisplayMode getMode() override;
                  std::list<Gokai::View::DisplayMode> getModes() override;
                  void setMode(Gokai::View::DisplayMode mode) override;
                  struct wlr_output* getValue();
                  std::shared_ptr<Gokai::Flutter::Engine> getEngine();

                  std::list<std::function<void()>> destroy;
                private:
                  std::shared_ptr<Gokai::Flutter::Engine> engine;
                  Gokai::Graphics::Renderer* renderer;
                  std::shared_ptr<Gokai::Context> context;
                  struct wlr_output* value;
                  uv_timer_t schedule_frame_handle;

                  struct wl_listener mode_listener;
                  struct wl_listener frame_listener;
                  struct wl_listener destroy_listener;

                  static void mode(struct wl_listener* listener, void* data);
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
