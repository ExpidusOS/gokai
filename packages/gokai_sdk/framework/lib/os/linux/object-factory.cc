#include <gokai/framework/os/linux/context.h>
#include <gokai/framework/os/linux/logging.h>
#include <gokai/framework/os/linux/object-factory.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux;

ObjectFactory::ObjectFactory(Gokai::ObjectArguments arguments) : Gokai::ObjectFactory(arguments) {}

Gokai::Object ObjectFactory::create(std::string name, Gokai::ObjectArguments arguments) {
  if (std::string(typeid(Gokai::Context).name()).compare(name) == 0) {
    return Context(arguments);
  }

  if (std::string(typeid(Gokai::Logger).name()).compare(name) == 0) {
    return Logger(arguments);
  }
  throw std::runtime_error("Invalid object type");
}

Gokai::Object* ObjectFactory::createPointer(std::string name, Gokai::ObjectArguments arguments) {
  if (std::string(typeid(Gokai::Context).name()).compare(name) == 0) {
    return new Context(arguments);
  }

  if (std::string(typeid(Gokai::Logger).name()).compare(name) == 0) {
    return new Logger(arguments);
  }
  return nullptr;
}

extern "C" Gokai::ObjectFactory* gokai_framework_object_factory_get_default() {
  static ObjectFactory* instance;
  if (instance == nullptr) {
    instance = new ObjectFactory(Gokai::ObjectArguments({}));
  }
  return instance;
}