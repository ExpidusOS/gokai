#pragma once

#ifdef __cplusplus
#include <gokai/graphics/renderer.h>
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Flutter {
    class Engine : public Gokai::Loggable {
      public:
        Engine(Gokai::ObjectArguments arguments);

        Gokai::Graphics::Renderer* getRenderer();
      private:
        Gokai::Graphics::Renderer* renderer;
    };
  }
}
#endif