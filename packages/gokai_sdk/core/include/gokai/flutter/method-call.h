#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Flutter {
    struct MethodCall {
      std::any arguments;
      std::string method;

      MethodCall();
      MethodCall(std::string method, std::any arguments);
    };
  }
}
#endif