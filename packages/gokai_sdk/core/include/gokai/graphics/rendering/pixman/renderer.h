#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>
#include <gokai/view/pixman/image.h>
#include <mutex>

namespace Gokai {
  namespace Graphics {
    namespace Rendering {
      namespace Pixman {
        class Renderer : public Gokai::Graphics::Renderer {
          public:
            Renderer(Gokai::ObjectArguments arguments);

            FlutterRendererConfig* getConfig() override;
            void resize(glm::uvec2 size);
            void render(Gokai::View::Displayable& target) override;
          private:
            uint32_t* buffer;
            glm::uvec2 size;
            pixman_image_t* img;

            FlutterRendererConfig config;
            std::mutex mx;

            static bool present(void* data, const void* buffer, size_t width, size_t height);
        };
      }
    }
  }
}
#endif