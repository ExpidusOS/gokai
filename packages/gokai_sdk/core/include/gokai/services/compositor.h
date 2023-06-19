#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class Compositor : public Gokai::Service, public Gokai::Loggable {
      public:
        Compositor(Gokai::ObjectArguments arguments);
        static const std::string SERVICE_NAME;
    };
  }
}
#endif