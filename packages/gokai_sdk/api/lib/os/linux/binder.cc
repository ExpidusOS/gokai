#include <gokai/api/os/linux/binder.h>
#include <gokai/values/string.h>
#include <stdexcept>

using namespace Gokai::API::os::Linux;

Binder::Binder(Gokai::ObjectArguments arguments) : Gokai::API::Binder(arguments) {
  this->path = static_cast<Gokai::Values::String*>(arguments.getPointed("path"))->getValue();
  if ((this->handle = dlopen(this->path.c_str(), 0)) == nullptr) {
    throw std::runtime_error("Failed to open shared library");
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