#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <gokai/logging.h>

namespace Gokai {
  namespace Input {
    class Touch : public Base, public Gokai::Loggable {
      public:
        Touch(Gokai::ObjectArguments arguments);
    };
  }
}
#endif