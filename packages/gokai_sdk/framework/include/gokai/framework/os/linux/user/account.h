#pragma once

#ifdef __cplusplus
#include <gokai/user/account.h>

extern "C" {
#include <act/act.h>
};

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace User {
          class Account : public Gokai::User::Account {
            public:
              Account(Gokai::ObjectArguments arguments);
              ~Account();

              Gokai::User::ID getId() override;
              std::locale getLanguage() override;

              ActUser* getValue();
            private:
              ActUser* value;
          };
        }
      }
    }
  }
}
#endif
