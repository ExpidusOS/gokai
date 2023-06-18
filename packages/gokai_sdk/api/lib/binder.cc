#include <gokai/api/binder.h>

using namespace Gokai::API;

Binder::Binder(Gokai::ObjectArguments arguments) : Object(arguments) {}

Gokai::ObjectFactory* Binder::getObjectFactory() {
  return nullptr;
}

std::string Binder::getPath() {
  return "";
}