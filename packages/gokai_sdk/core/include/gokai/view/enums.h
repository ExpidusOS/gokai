#pragma once

#ifdef __cplusplus
namespace Gokai {
  namespace View {
    enum Side {
      left,
      right,
      top,
      bottom
    };

    enum Corner {
      top_left,
      top_right,
      bottom_left,
      bottom_right,
    };

    enum HDRType {
      dolby_vision,
      hdr10,
      hdr10_plus,
      hlg,
    };
  }
}
#endif