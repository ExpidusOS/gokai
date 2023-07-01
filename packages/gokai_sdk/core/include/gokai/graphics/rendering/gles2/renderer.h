#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace GLES2 {
        class Renderer : public Gokai::Graphics::Renderer {
          public:
            Renderer(Gokai::ObjectArguments arguments);

            FlutterRendererConfig* getConfig() override;
          protected:
            void init();
          private:
            FlutterRendererConfig config;

            static bool present_with_info_callback(void* data, const FlutterPresentInfo* info);
            static uint32_t fbo_callback(void* data);
        };
      }
    }
  }
}
#endif