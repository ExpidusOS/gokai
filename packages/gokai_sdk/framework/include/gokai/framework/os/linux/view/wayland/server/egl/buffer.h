#pragma once

#ifdef __cplusplus
#include <gokai/view/displayable.h>
#include <gokai/logging.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

extern "C" {
#define static
#include <wlr/render/allocator.h>
#include <wlr/render/drm_format_set.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/render/wlr_texture.h>
#include <wlr/types/wlr_buffer.h>
#undef static
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace View {
          namespace Wayland {
            namespace Server {
              namespace EGL {
                class Buffer : public Gokai::View::Displayable, public Gokai::Loggable {
                  public:
                    Buffer(Gokai::ObjectArguments arguments);
                    ~Buffer();

                    Gokai::View::URect getRect() override;
                    struct wlr_texture* getTexture();
                    struct wlr_buffer* getBuffer();
                    GLuint getFbo();
                  private:
                    struct wlr_renderer* renderer;
                    struct wlr_texture* texture;
                    struct wlr_buffer* buffer;
                    GLuint fbo;
                };
              }
            }
          }
        }
      }
    }
  }
}
#endif