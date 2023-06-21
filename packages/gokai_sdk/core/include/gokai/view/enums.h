#pragma once

#ifdef __cplusplus
#include <stdint.h>

namespace Gokai {
  namespace View {
    enum Side : uint8_t {
      left = 0,
      right = 1,
      top = 2,
      bottom = 3
    };

    enum Corner : uint8_t {
      top_left = 0,
      top_right = 1,
      bottom_left = 2,
      bottom_right = 3,
    };

    enum HDRType : uint8_t {
      dolby_vision = 0,
      hdr10 = 1,
      hdr10_plus = 2,
      hlg = 3,
    };
  }
}
#endif
