#pragma once

#ifdef __cplusplus
#include <gokai/services/account-manager.h>

extern "C" {
#include <act/act.h>
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class AccountManager : public Gokai::Services::AccountManager {
            public:
              AccountManager(Gokai::ObjectArguments arguments);
              ~AccountManager();

              std::list<Gokai::User::ID> getIds() override;
            private:
              ActUserManager* manager;
              gulong user_changed_id;

              static void user_changed(ActUser* user, gpointer user_data);
          };
        }
      }
    }
  }
}
#endif
