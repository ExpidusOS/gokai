#include <gokai/input/base.h>

using namespace Gokai::Input;

Base::Base(Gokai::ObjectArguments arguments) : Object(arguments) {
  auto context = std::any_cast<std::shared_ptr<Gokai::Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;
}

std::string Base::getName() {
  return std::string();
}