#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class WindowManager : public Gokai::Service {
      public:
        WindowManager(Gokai::Context* context);
      public:
        static const std::string SERVICE_NAME;
    };
  }
}
#endif