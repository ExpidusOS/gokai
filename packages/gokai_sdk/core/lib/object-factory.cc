#include <gokai/object-factory.h>
#include <stdexcept>

using namespace Gokai;

ObjectFactory::ObjectFactory(ObjectArguments arguments) : Object(arguments) {}

Object ObjectFactory::create(std::string name, ObjectArguments arguments) {
  throw std::runtime_error("ObjectFactory::create is not implemented");
}

Object* ObjectFactory::createPointer(std::string name, ObjectArguments arguments) {
  return nullptr;
}