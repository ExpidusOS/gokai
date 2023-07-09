#pragma once

#ifdef __cplusplus
#include <gokai/user/id.h>
#include <gokai/object.h>
#include <locale>

namespace Gokai {
  namespace User {
    class Account : public Gokai::Object {
      public:
        Account(Gokai::ObjectArguments arguments);

        virtual ID getId();
        virtual std::locale getLanguage();
        virtual std::string getDisplayName();
        virtual std::string getPicture();
        virtual bool isSystem();
        virtual bool isAdministrator();
    };
  }
}
#endif
