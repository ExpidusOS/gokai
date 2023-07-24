#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <gokai/logging.h>

namespace Gokai {
  namespace Input {
    class Switch : public Base, public Gokai::Loggable {
      public:
        Switch(Gokai::ObjectArguments arguments);
    };
  }
}
#endif
