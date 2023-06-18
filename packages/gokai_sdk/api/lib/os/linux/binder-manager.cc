#include <gokai/api/os/linux/binder.h>
#include <gokai/api/os/linux/binder-manager.h>
#include <gokai/values/string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <filesystem>
#include <gokai-api-build.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace Gokai::API::os::Linux;

static std::filesystem::path get_runtime_libdir() {
  struct stat sb;
  lstat("/proc/self/exe", &sb);

  size_t linksize = sb.st_size + 1;
  if (linksize == 0) linksize = PATH_MAX;

  char* link_path = reinterpret_cast<char*>(malloc(linksize));
  assert(link_path != nullptr);

  readlink("/proc/self/exe", link_path, linksize);
  return std::filesystem::path(link_path).parent_path() / "lib" / "gokai" / "frameworks";
}

BinderManager::BinderManager(Gokai::ObjectArguments arguments) : Gokai::API::BinderManager(arguments) {
  if (this->binder_default == nullptr && this->is_portable) {
    for (const auto& entry : std::filesystem::directory_iterator(get_runtime_libdir())) {
      if (entry.path().extension().compare(".so") != 0) continue;

      this->binder_default = this->load(entry.path());
      break;
    }
  }
}

Gokai::API::Binder* BinderManager::load(std::string name) {
  if (this->isCached(name)) return this->getCached(name);

  auto value = new Binder(Gokai::ObjectArguments({
    { "path", Gokai::Values::String(name) },
  }));
  this->binder_cache[name] = value;
  return value;
}

std::map<std::string, Gokai::API::Binder*> BinderManager::getAll() {
  auto basepaths = std::vector<std::string>();
  basepaths.push_back(std::string(GOKAI_API_LIBDIR) + "/gokai/frameworks");
  basepaths.push_back(get_runtime_libdir());

  for (auto basepath : basepaths) {
    struct stat info;
    if (stat(basepath.c_str(), &info) != 0) continue;

    for (const auto& entry : std::filesystem::directory_iterator(basepath)) {
      if (entry.path().extension().compare(".so") != 0) continue;
      if (this->isCached(entry.path())) continue;

      auto value = new Binder(Gokai::ObjectArguments({
        { "path", Gokai::Values::String(entry.path()) },
      }));
      this->binder_cache[entry.path()] = value;
    }
  }
  return this->binder_cache;
}

Gokai::API::BinderManager* Gokai::API::BinderManager::create(ObjectArguments arguments) {
  return new BinderManager(arguments);
}