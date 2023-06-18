#pragma once

#ifdef __cplusplus
#include <gokai/value.h>

namespace Gokai {
  namespace Values {
    class Boolean : public Gokai::Value {
      public:
        Boolean(bool value);
        void* toPointer() override;
        bool getValue();
      private:
        bool value;
    };
  }
}
#endif