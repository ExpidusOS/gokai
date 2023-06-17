#include <gokai/value.h>

using namespace Gokai;

bool Value::isSet() {
  return this->toPointer() != nullptr;
}

bool Value::isEmpty() {
  return this->toPointer() != nullptr;
}

void* Value::toPointer() {
  return nullptr;
}