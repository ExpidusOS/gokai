#include <gokai/os/linux/dynamic-library.h>
#include <fmt/core.h>
#include <dlfcn.h>
#include <stdexcept>

using namespace Gokai::os::Linux;

DynamicLibrary::DynamicLibrary(Gokai::ObjectArguments arguments) : Gokai::os::DynamicLibrary(arguments) {
  char* path = nullptr;
  if (arguments.has("path")) {
    path = (char*)(std::any_cast<std::string>(arguments.get("path")).c_str());
  }

  this->handle = dlopen(path, RTLD_LAZY);
  if (this->handle == nullptr) {
    throw std::runtime_error(fmt::format("Failed to open \"{}\": {}", path, dlerror()));
  }
}

DynamicLibrary::~DynamicLibrary() {
  dlclose(this->handle);
}

void* DynamicLibrary::lookup(std::string sym) {
  return dlsym(this->handle, sym.c_str());
}

bool DynamicLibrary::providesSymbol(std::string sym) {
  return dlsym(this->handle, sym.c_str()) != nullptr;
}

Gokai::os::DynamicLibrary* Gokai::os::DynamicLibrary::open(std::string path) {
  return new Gokai::os::Linux::DynamicLibrary(Gokai::ObjectArguments({
    { "path", path },
  }));
}
