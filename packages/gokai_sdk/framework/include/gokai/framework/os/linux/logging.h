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

            std::shared_ptr<spdlog::logger> get(const char* tag, Gokai::ObjectArguments arguments) override;
          private:
            std::shared_ptr<spdlog::logger> get(const char* tag, std::string type);
        };
      }
    }
  }
}
#endif