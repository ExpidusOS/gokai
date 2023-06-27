#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <gokai/logging.h>

namespace Gokai {
  namespace Input {
    class Keyboard : public Base, public Gokai::Loggable {
      public:
        Keyboard(Gokai::ObjectArguments arguments);
    };
  }
}
#endif