#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/services/package-manager.h>
#include <gokai/services/engine-manager.h>
#include <gokai/service.h>
#include <basedir.h>
#include <string>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        struct ContextDisplayBackend {
          uint8_t id;
          std::string name;

          ContextDisplayBackend();
          ContextDisplayBackend(uint8_t id, std::string name);

          bool operator!=(ContextDisplayBackend b);
          bool operator==(ContextDisplayBackend b);

          static ContextDisplayBackend fromValue(std::any value);

          static const ContextDisplayBackend try_auto;
          static const ContextDisplayBackend wayland;
          static const ContextDisplayBackend x11;
          static const ContextDisplayBackend values[3];
        };

        class Context : public Gokai::Context {
          public:
            Context(Gokai::ObjectArguments arguments);
            ~Context();

            ContextDisplayBackend getDisplayBackend();
            std::string getPackageDir() override;
            std::string getPackageConfigDir() override;
          private:
            ContextDisplayBackend display_backend;
            xdgHandle xdg_handle;
        };
      }
    }
  }
}
#endif