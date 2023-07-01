#pragma once

#ifdef __cplusplus
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Graphics {
    class Compositor : public Gokai::Loggable {
      public:
        Compositor(Gokai::ObjectArguments arguments);

        virtual FlutterCompositor* get();
    };
  }
}
#endif