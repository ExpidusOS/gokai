#pragma once

#ifdef __cplusplus
#include <gokai/object.h>

namespace Gokai {
  namespace Input {
    class Base : public Gokai::Object {
      public:
        Base(Gokai::ObjectArguments arguments);

        virtual std::string getName();
    };
  }
}
#endif