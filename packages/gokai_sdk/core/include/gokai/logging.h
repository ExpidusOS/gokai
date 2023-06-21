#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <spdlog/spdlog.h>

namespace Gokai {
  class Logger : public Object {
    public:
      Logger(ObjectArguments arguments);

      virtual std::shared_ptr<spdlog::logger> get(const char* tag, ObjectArguments arguments);
#ifdef TAG
      inline std::shared_ptr<spdlog::logger> get() {
        return get(TAG, ObjectArguments({}));
      }
#endif
    protected:
      std::shared_ptr<spdlog::logger> logger;
  };

  class Loggable : public Object {
    public:
      std::shared_ptr<spdlog::logger> logger;
      Loggable(const char* tag, ObjectArguments arguments);
      ~Loggable();

      Logger* getLogger();
    private:
      Logger* obj;
  };
}
#endif