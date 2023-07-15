#pragma once

#ifdef __cplusplus
#include <gokai/flutter/engine.h>
#include <gokai/flutter/texture.h>
#include <gokai/object.h>

namespace Gokai {
  namespace Graphics {
    class Texture : public Gokai::Object {
      public:
        Texture(Gokai::ObjectArguments arguments);

        std::list<std::function<void()>> onFrame;
        virtual bool frame(Gokai::Flutter::Engine* engine, size_t width, size_t height, Gokai::Flutter::Texture* out);
    };
  }
}
#endif