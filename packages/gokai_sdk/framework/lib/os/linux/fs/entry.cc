#include <gokai/framework/os/linux/fs/entry.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::FS;

Entry::Entry(Gokai::ObjectArguments arguments) : Gokai::FS::Entry(arguments) {
  GVfs* vfs = nullptr;

  if (arguments.has("vfs")) {
    vfs = std::any_cast<GVfs*>(arguments.get("vfs"));
  } else {
    vfs = g_vfs_get_default();
  }

  if (vfs == nullptr) {
    throw std::runtime_error("VFS was not properly given");
  }

  this->value = g_vfs_get_file_for_path(vfs, this->getPath().c_str());
  if (this->value == nullptr) {
    throw std::runtime_error("Could not get a file");
  }
}

Entry::~Entry() {
  g_clear_object(&this->value);
}

GFile* Entry::getValue() {
  return this->value;
}
