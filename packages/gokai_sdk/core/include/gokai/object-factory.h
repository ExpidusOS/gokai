#pragma once

#ifdef __cplusplus
#include <gokai/object.h>

namespace Gokai {
  class ObjectFactory : public Object {
    public:
      ObjectFactory(ObjectArguments arguments);

      virtual Object create(std::string name, ObjectArguments arguments);
      virtual Object* createPointer(std::string name, ObjectArguments arguments);
  };
}
#endif