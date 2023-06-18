#include <gokai/values/string.h>

using namespace Gokai::Values;

String::String(std::string value) {
  this->value = value;
}

void* String::toPointer() {
  return &this->value;
}

std::string String::getValue() {
  return this->value;
}