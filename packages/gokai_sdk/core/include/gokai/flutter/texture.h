#pragma once

#ifdef __cplusplus
#include <flutter_embedder.h>

namespace Gokai {
  namespace Flutter {
    union Texture {
      FlutterOpenGLTexture open_gl;
      FlutterMetalExternalTexture metal;
    };
  }
}
#endif