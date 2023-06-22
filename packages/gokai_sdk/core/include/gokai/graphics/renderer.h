#pragma once

#ifdef __cplusplus
#include <gokai/view/displayable.h>
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Graphics {
    class Renderer : public Gokai::Loggable {
      public:
        Renderer(Gokai::ObjectArguments arguments);

        virtual FlutterRendererConfig* getConfig();
        virtual void render(Gokai::View::Displayable& target);
    };
  }
}
#endif