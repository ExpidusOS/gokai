#pragma once

#ifdef __cplusplus
#include <gokai/services/account-manager.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class AccountManager : public Gokai::Services::AccountManager {
            public:
              AccountManager(Gokai::ObjectArguments arguments);
          };
        }
      }
    }
  }
}
#endif
