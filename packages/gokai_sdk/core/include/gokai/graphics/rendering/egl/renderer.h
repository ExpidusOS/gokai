#pragma once

#ifdef __cplusplus
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gokai/graphics/rendering/gles2/renderer.h>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace EGL {
        class Renderer : public Gokai::Graphics::Rendering::GLES2::Renderer {
          public:
            Renderer(EGLDisplay display, EGLContext context, Gokai::ObjectArguments arguments);

            FlutterRendererConfig* getConfig() override;
          private:
            EGLDisplay display;
            EGLContext context;
        };
      }
    }
  }
}
#endif