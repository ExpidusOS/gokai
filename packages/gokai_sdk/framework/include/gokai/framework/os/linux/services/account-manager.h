#pragma once

#ifdef __cplusplus
#include <gokai/services/account-manager.h>
#include <gokai/user/account.h>

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

              Gokai::User::ID getCurrentId() override;
              std::list<Gokai::User::ID> getIds() override;
              Gokai::User::Account* get(Gokai::User::ID id) override;
            private:
              std::map<Gokai::User::ID, Gokai::User::Account*> cache;
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
