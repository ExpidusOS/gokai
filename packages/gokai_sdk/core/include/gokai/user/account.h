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
    };
  }
}
#endif
