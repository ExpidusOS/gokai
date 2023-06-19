#pragma once

#ifdef __cplusplus
#include <gokai/policies.h>
#include <gokai/value.h>
#include <any>
#include <map>
#include <string>

namespace Gokai {
  class ObjectArguments {
    public:
      ObjectArguments(std::map<std::string, Value<std::any>> arguments, TypeSafePolicy policy);
      ObjectArguments(std::map<std::string, Value<std::any>> arguments);

      bool has(std::string name);

      Value<std::any> get(std::string name);
      Value<std::any> getAssert(std::string name);
      Value<std::any> getException(std::string name);
      Value<std::any> getUnsafe(std::string name);
    private:
      std::map<std::string, Value<std::any>> arguments;
      TypeSafePolicy policy;
  };
}
#endif