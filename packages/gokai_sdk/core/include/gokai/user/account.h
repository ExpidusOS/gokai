#pragma once

#ifdef __cplusplus
#include <gokai/user/id.h>
#include <gokai/context.h>
#include <gokai/object.h>
#include <locale>
#include <functional>
#include <list>

namespace Gokai {
  namespace User {
    class Account : public Gokai::Object {
      public:
        Account(Gokai::ObjectArguments arguments);

        std::list<std::function<void()>> onChange;

        virtual ID getId();
        virtual std::locale getLanguage();
        virtual std::string getDisplayName();
        virtual std::string getPicture();
        virtual std::string getHome();
        virtual bool isSystem();
        virtual bool isAdministrator();
      protected:
        std::shared_ptr<Gokai::Context> context;
    };
  }
}
#endif
