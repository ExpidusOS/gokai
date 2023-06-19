#include <gokai/application-manifest.h>

using namespace Gokai;

ApplicationManifest::ApplicationManifest(YAML::Node node) {
  this->id = node["id"].as<std::string>();

  if (node["defaults"].IsDefined()) {
    this->defaults = node["defaults"].as<std::map<std::string, std::string>>();
  }

  if (node["overrides"].IsDefined()) {
    this->overrides = node["overrides"].as<std::map<std::string, std::string>>();
  }
}