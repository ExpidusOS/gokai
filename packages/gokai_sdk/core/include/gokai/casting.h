#pragma once

#ifdef __cplusplus
#include <any>
#include <stdint.h>

namespace Gokai {
  namespace Casting {
    uint64_t castInt(std::any value);
  }
}
#endif
