#include <gokai/input/base.h>

using namespace Gokai::Input;

Base::Base(Gokai::ObjectArguments arguments) : Object(arguments) {}

std::string Base::getName() {
  return std::string();
}