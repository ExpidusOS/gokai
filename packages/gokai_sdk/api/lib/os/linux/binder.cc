#include <gokai/api/os/linux/binder.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

using namespace Gokai::API::os::Linux;

Binder::Binder(Gokai::ObjectArguments arguments) : Gokai::API::Binder(arguments) {
  this->path = std::any_cast<std::string>(arguments.get("path"));

  if ((this->handle = dlopen(this->path.c_str(), RTLD_NOW | RTLD_GLOBAL)) == nullptr) {
    auto message = std::string("Failed to open shared library: ") + dlerror();
    spdlog::error(message);
    throw std::runtime_error(message);
  }
}

Binder::~Binder() {
  delete this->getObjectFactory();
  dlclose(this->handle);
}

Gokai::ObjectFactory* Binder::getObjectFactory() {
  void* sym = dlsym(this->handle, "gokai_framework_object_factory_get_default");
  if (sym == nullptr) {
    throw std::runtime_error("Failed to get function binding for \"gokai_framework_object_factory_get_default\"");
  }

  return reinterpret_cast<Gokai::GetObjectFactoryDefaultFunc>(sym)();
}

std::string Binder::getPath() {
  return this->path;
}