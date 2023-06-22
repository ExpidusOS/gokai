#pragma once

#ifdef __cplusplus
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Graphics {
    class Renderer : public Gokai::Loggable {
      public:
        Renderer(Gokai::ObjectArguments arguments);

        virtual FlutterRendererConfig* getConfig();
    };
  }
}
#endif