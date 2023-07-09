#include <gokai/os/dynamic-library.h>

using namespace Gokai::os;

DynamicLibrary::DynamicLibrary(Gokai::ObjectArguments arguments) : Object(arguments) {}

void* DynamicLibrary::lookup(std::string sym) {
  return nullptr;
}

bool DynamicLibrary::providesSymbol(std::string sym) {
  return false;
}
