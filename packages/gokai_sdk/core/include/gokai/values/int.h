#pragma once

#ifdef __cplusplus
#include <gokai/value.h>

namespace Gokai {
  namespace Values {
    class Integer : public Gokai::Value {
      public:
        Integer(int value);
        void* toPointer() override;
        int getValue();
      private:
        int value;
    };
  }
}
#endif