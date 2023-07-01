#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>
#include <GLES2/gl2.h>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace GLES2 {
        class Renderer : public Gokai::Graphics::Renderer {
          public:
            Renderer(Gokai::ObjectArguments arguments);

            FlutterRendererConfig* getConfig() override;
          private:
            FlutterRendererConfig config;

            static uint32_t fbo_with_frame_info_callback(void* data, const FlutterFrameInfo* info);
        };
      }
    }
  }
}
#endif