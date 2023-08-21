#include <gokai/framework/os/linux/fs/entry.h>
#include <gokai/framework/os/linux/services/storage-manager.h>

using namespace Gokai::Framework::os::Linux::Services;

StorageManager::StorageManager(Gokai::ObjectArguments arguments) : Gokai::Services::StorageManager(arguments) {}

Gokai::FS::Entry StorageManager::open(std::filesystem::path path) {
  return Gokai::Framework::os::Linux::FS::Entry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", path },
  }));
}
