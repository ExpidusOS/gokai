#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <appstream.h>
#include <basedir.h>
#include <string>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        class Context : public Gokai::Context {
          public:
            Context(Gokai::ObjectArguments arguments);
            ~Context();

            Service* getSystemService(std::string serviceName) override;
            std::string getPackageName() override;
            std::string getPackageDir() override;
          private:
            AsMetadata* metadata;
            xdgHandle xdg_handle;
        };
      }
    }
  }
}
#endif