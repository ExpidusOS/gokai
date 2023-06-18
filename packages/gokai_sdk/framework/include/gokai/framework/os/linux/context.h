#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/services/package-manager.h>
#include <gokai/context.h>
#include <gokai/service.h>
#include <appstream.h>
#include <basedir.h>
#include <string>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        enum ContextType {
          GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_AUTO = (0 << 0),
          GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_CLIENT = (1 << 0),
          GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_COMPOSITOR = (2 << 0),
          GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_WAYLAND = (0 << 3),
          GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_X11 = (1 << 3),
        };

        class Context : public Gokai::Context {
          public:
            Context(Gokai::ObjectArguments arguments);
            ~Context();

            Service* getSystemService(std::string serviceName) override;
            std::string getPackageName() override;
            std::string getPackageConfigDir() override;
            std::string getPackageDataDir() override;
          private:
            AsMetadata* metadata;
            xdgHandle xdg_handle;
            Services::PackageManager* package_manager;
            std::map<std::string, Gokai::Service*> services;
            ContextType type;
        };
      }
    }
  }
}
#endif