#pragma once

#ifdef __cplusplus
#include <gokai/policies.h>
#include <any>
#include <map>
#include <string>

namespace Gokai {
  class ObjectArguments {
    public:
      ObjectArguments(std::map<std::string, std::any> arguments, TypeSafePolicy policy);
      ObjectArguments(std::map<std::string, std::any> arguments);

      bool has(std::string name);

      std::any get(std::string name);
      std::any getAssert(std::string name);
      std::any getException(std::string name);
      std::any getUnsafe(std::string name);
    private:
      std::map<std::string, std::any> arguments;
      TypeSafePolicy policy;
  };
}
#endif