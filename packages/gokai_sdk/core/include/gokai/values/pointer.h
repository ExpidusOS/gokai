#pragma once

#ifdef __cplusplus
#include <gokai/value.h>

namespace Gokai {
  namespace Values {
    class Pointer : public Gokai::Value {
      public:
        Pointer(void* ptr);
        void* toPointer() override;
      private:
        void* value;
    };
  }
}
#endif