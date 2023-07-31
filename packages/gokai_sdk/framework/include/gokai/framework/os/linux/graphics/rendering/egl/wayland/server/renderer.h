#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/view/wayland/server/egl/buffer.h>
#include <gokai/graphics/renderer.h>
#include <gokai/context.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

extern "C" {
#define static
#include <wlr/render/allocator.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Graphics {
          namespace Rendering {
            namespace EGL {
              namespace Wayland {
                namespace Server {
                  class Renderer : public Gokai::Graphics::Renderer {
                    public:
                      Renderer(Gokai::ObjectArguments arguments);
                      ~Renderer();

                      Gokai::Framework::os::Linux::View::Wayland::Server::EGL::Buffer* buffer;
                      std::list<Gokai::View::DRect> damage;

                      FlutterRendererConfig* getConfig() override;
                      void resize(glm::uvec2 size) override;
                    private:
                      std::shared_ptr<Gokai::Context> gk_context;
                      EGLContext context;
                      EGLContext resource_context;

                      FlutterRendererConfig config;
                      struct wlr_renderer* value;

                      static bool make_current_callback(void* data);
                      static bool clear_current_callback(void* data);
                      static bool make_resource_current_callback(void* data);
                      static uint32_t fbo_with_frame_info_callback(void* data, const FlutterFrameInfo* info);
                      static bool present_with_info_callback(void* data, const FlutterPresentInfo* info);
                      static bool gl_external_texture_frame_callback(void* data, int64_t tid, size_t width, size_t height, FlutterOpenGLTexture* out);
                      static FlutterTransformation surface_transformation(void* data);
                      static void populate_existing_damage(void* data, intptr_t fbo_id, FlutterDamage* damage);
                  };
                }
              }
            }
          }
        }
      }
    }
  }
}
#endif
