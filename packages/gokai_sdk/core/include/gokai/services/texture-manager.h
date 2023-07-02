#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class TextureManager : public Gokai::Service, public Gokai::Loggable {
      public:
        TextureManager(Gokai::ObjectArguments arguments);

        int64_t allocate();
        void unregister(int64_t id);

        static const std::string SERVICE_NAME;
      private:
        int64_t next_id;
    };
  }
}
#endif