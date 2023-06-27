#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <gokai/logging.h>

namespace Gokai {
  namespace Input {
    class Pointer : public Base, public Gokai::Loggable {
      public:
        Pointer(Gokai::ObjectArguments arguments);
    };
  }
}
#endif