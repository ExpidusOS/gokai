#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <gokai/object-factory.h>
#include <string>

namespace Gokai {
  namespace API {
    class Binder : public Gokai::Object {
      public:
        Binder(ObjectArguments arguments);

        virtual Gokai::ObjectFactory* getObjectFactory();
        virtual std::string getPath();
    };
  }
}
#endif