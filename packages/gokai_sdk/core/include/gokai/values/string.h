#pragma once

#ifdef __cplusplus
#include <gokai/value.h>
#include <string>

namespace Gokai {
  namespace Values {
    class String : public Gokai::Value {
      public:
        String(std::string value);
        void* toPointer() override;
        std::string getValue();
      private:
        std::string value;
    };
  }
}
#endif