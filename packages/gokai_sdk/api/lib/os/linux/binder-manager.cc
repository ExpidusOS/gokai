#include <gokai/api/os/linux/binder.h>
#include <gokai/api/os/linux/binder-manager.h>
#include <gokai/os/paths.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <filesystem>
#include <gokai-api-build.h>
#include <limits.h>
#include <stdlib.h>
#include <type_traits>
#include <unistd.h>
#include <vector>

using namespace Gokai::API::os::Linux;

BinderManager::BinderManager(Gokai::ObjectArguments arguments) : Gokai::API::BinderManager(arguments) {
  if (this->binder_default == nullptr) {
    spdlog::debug("Trying to load from application's runtime directory");

    auto runtime_dir = std::filesystem::path(Gokai::os::Paths::getRuntimePath()) / "lib" / "gokai" / "frameworks";
    struct stat info;
    if (stat(runtime_dir.c_str(), &info) == 0) {
      spdlog::debug("Loading from {}", runtime_dir.c_str());
      for (const auto& entry : std::filesystem::directory_iterator(runtime_dir)) {
        if (entry.path().extension().compare(".so") != 0) continue;

        this->binder_default = this->load(entry.path());
        break;
      }
    } else {
      spdlog::warn("Was not able to load frameworks from {} due to not existing.", runtime_dir.c_str());
    }
  }

  if (this->binder_default == nullptr) {
    spdlog::debug("Gokai API binder has not been set, searching for the first one.");
    auto all = this->getAll();
    auto first = all.begin();
    if (first != all.end()) {
      this->binder_default = first->second;
    }
  }

  if (this->binder_default != nullptr) {
    spdlog::debug("Found the default Gokai library at {}", this->binder_default->getPath().c_str());
  }
}

Gokai::API::Binder* BinderManager::load(std::string name) {
  if (this->isCached(name)) return this->getCached(name);

  auto value = new Binder(Gokai::ObjectArguments({
    { "path", name },
  }));
  this->binder_cache[name] = value;
  return value;
}

std::map<std::string, Gokai::API::Binder*> BinderManager::getAll() {
  auto basepaths = std::vector<std::string>();
  basepaths.push_back(std::string(GOKAI_API_LIBDIR) + "/gokai/frameworks");
  basepaths.push_back(std::filesystem::path(Gokai::os::Paths::getRuntimePath()) / "lib" / "gokai" / "frameworks");

  for (auto basepath : basepaths) {
    struct stat info;
    if (stat(basepath.c_str(), &info) != 0) continue;

    spdlog::debug("Loading from {}", basepath.c_str());
    for (const auto& entry : std::filesystem::directory_iterator(basepath)) {
      if (entry.path().extension().compare(".so") != 0) continue;
      if (this->isCached(entry.path())) continue;

      auto value = new Binder(Gokai::ObjectArguments({
        { "path", std::make_any<std::string>(entry.path()) },
      }));
      this->binder_cache[entry.path()] = value;
    }
  }
  return this->binder_cache;
}

Gokai::API::BinderManager* Gokai::API::BinderManager::create(ObjectArguments arguments) {
  return new Gokai::API::os::Linux::BinderManager(arguments);
}
