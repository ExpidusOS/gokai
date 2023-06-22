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

        xg::Guid getId();
        Gokai::Graphics::Renderer* getRenderer();
        Gokai::Context* getContext();
      private:
        xg::Guid id;
        Gokai::Graphics::Renderer* renderer;
        Gokai::Context* context;
    };
  }
}
#endif