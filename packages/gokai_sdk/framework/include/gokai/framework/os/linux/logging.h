#pragma once

#ifdef __cplusplus
#include <gokai/logging.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        class Logger : public Gokai::Logger {
          public:
            Logger(Gokai::ObjectArguments arguments);

            std::shared_ptr<spdlog::logger> get(const char* tag) override;
        };
      }
    }
  }
}
#endif