#pragma once

#ifdef __cplusplus
#include <yaml-cpp/yaml.h>
#include <any>
#include <list>
#include <string>

namespace Gokai {
  struct ApplicationManifest {
    std::string id;
    std::map<std::string, std::string> defaults;
    std::map<std::string, std::string> overrides;

    ApplicationManifest(YAML::Node node);
  };
}
#endif