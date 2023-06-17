#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <gokai/value.h>
#include <string>

namespace Gokai {
  namespace API {
    class Binder {
      public:
        Binder(std::string libpath);
        ~Binder();
    };
  }
}
#endif