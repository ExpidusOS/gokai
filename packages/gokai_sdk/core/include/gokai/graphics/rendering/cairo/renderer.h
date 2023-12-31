#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>
#include <gokai/view/cairo/image.h>
#include <mutex>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace Cairo {
        class Renderer : public Gokai::Graphics::Renderer {
          public:
            Renderer(Gokai::ObjectArguments arguments);

            FlutterRendererConfig* getConfig() override;
            glm::uvec2 getSize() override;
            void resize(glm::uvec2 size) override;
            void render(Gokai::View::Displayable& target) override;
          private:
            cairo_surface_t* surface;

            FlutterRendererConfig config;
            std::mutex mx;

            static bool present(void* data, const void* buffer, size_t width, size_t height);
        };
      }
    }
  }
}
#endif