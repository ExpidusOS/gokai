#pragma once

#ifdef __cplusplus
#include <gokai/policies.h>
#include <gokai/value.h>
#include <map>
#include <string>

namespace Gokai {
  class ObjectArguments {
    public:
      ObjectArguments(std::map<std::string, Value> arguments, TypeSafePolicy policy);
      ObjectArguments(std::map<std::string, Value> arguments);

      bool has(std::string name);

      Value get(std::string name);
      Value getAssert(std::string name);
      Value getException(std::string name);
      Value getUnsafe(std::string name);
    private:
      std::map<std::string, Value> arguments;
      TypeSafePolicy policy;
  };
}
#endif