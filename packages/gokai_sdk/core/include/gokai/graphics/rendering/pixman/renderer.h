#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace Pixman {
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