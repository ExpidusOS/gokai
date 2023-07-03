#pragma once

#ifdef __cplusplus
#include <gokai/flutter/texture.h>
#include <gokai/object.h>

namespace Gokai {
  namespace Graphics {
    class Texture : public Gokai::Object {
      public:
        Texture(Gokai::ObjectArguments arguments);

        virtual bool frame(size_t width, size_t height, Gokai::Flutter::Texture* out);
    };
  }
}
#endif