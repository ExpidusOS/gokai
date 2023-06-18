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
        class ContextDisplayBackend {
          public:
            ContextDisplayBackend();
            ContextDisplayBackend(uint8_t id, std::string name);

            bool operator!=(ContextDisplayBackend b);
            bool operator==(ContextDisplayBackend b);

            static ContextDisplayBackend fromValue(Value* value);

            static const ContextDisplayBackend try_auto;
            static const ContextDisplayBackend wayland;
            static const ContextDisplayBackend x11;
            static const ContextDisplayBackend values[3];
          private:
            uint8_t id;
            std::string name;
        };

        class Context : public Gokai::Context {
          public:
            Context(Gokai::ObjectArguments arguments);
            ~Context();

            ContextDisplayBackend getDisplayBackend();
            Service* getSystemService(std::string serviceName) override;
            std::string getPackageName() override;
            std::string getPackageConfigDir() override;
            std::string getPackageDataDir() override;
          private:
            ContextDisplayBackend display_backend;
            AsMetadata* metadata;
            xdgHandle xdg_handle;
            Services::PackageManager* package_manager;
            std::map<std::string, Gokai::Service*> services;
        };
      }
    }
  }
}
#endif