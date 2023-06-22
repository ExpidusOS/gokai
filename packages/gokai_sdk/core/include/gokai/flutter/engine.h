#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/graphics/renderer.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Flutter {
    class Engine : public Gokai::Loggable {
      public:
        Engine(Gokai::ObjectArguments arguments);
        ~Engine();

        xg::Guid getId();
        Gokai::Graphics::Renderer* getRenderer();
        Gokai::Context* getContext();
        FlutterEngine getValue();
        void resize(glm::uvec2 size);
      private:
        xg::Guid id;
        FlutterProjectArgs args;
        FlutterEngine value;

        Gokai::Graphics::Renderer* renderer;
        Gokai::Context* context;
    };
  }
}
#endif