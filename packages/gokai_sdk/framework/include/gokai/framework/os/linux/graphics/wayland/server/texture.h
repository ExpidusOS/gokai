#pragma once

#ifdef __cplusplus
#include <gokai/graphics/surface-buffer-chain.h>
#include <gokai/graphics/texture.h>

extern "C" {
#define static
#include <wlr/render/gles2.h>
#include <wlr/render/wlr_texture.h>
#include <wlr/types/wlr_buffer.h>
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Graphics {
          namespace Wayland {
            namespace Server {
              class Texture : public Gokai::Graphics::Texture {
                public:
                  Texture(Gokai::ObjectArguments arguments);

                  Gokai::Graphics::SurfaceBufferChain<struct wlr_buffer> chain;

                  void commit(struct wlr_buffer* buffer);
                  bool frame(Gokai::Flutter::Engine* engine, size_t width, size_t height, Gokai::Flutter::Texture* out) override;
              };
            }
          }
        }
      }
    }
  }
}
#endif