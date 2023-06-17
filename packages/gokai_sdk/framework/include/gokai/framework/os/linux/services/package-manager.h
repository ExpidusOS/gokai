#pragma once

#ifdef __cplusplus
#include <gokai/services/package-manager.h>
#include <packagekit-glib2/packagekit.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class PackageManager : public Gokai::Services::PackageManager {
            public:
              PackageManager(Gokai::ObjectArguments arguments);
              ~PackageManager();
            private:
              PkClient* client;
          };
        }
      }
    }
  }
}
#endif