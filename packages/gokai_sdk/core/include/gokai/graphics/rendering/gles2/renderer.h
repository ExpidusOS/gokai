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
        };
      }
    }
  }
}
#endif