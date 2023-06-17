#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class DisplayManager : public Gokai::Service {
      public:
        DisplayManager(Gokai::ObjectArguments arguments);
        static const std::string SERVICE_NAME;
    };
  }
}
#endif