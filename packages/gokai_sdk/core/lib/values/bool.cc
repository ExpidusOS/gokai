#include <gokai/values/bool.h>

using namespace Gokai::Values;

Boolean::Boolean(bool value) {
  this->value = value;
}

void* Boolean::toPointer() {
  return reinterpret_cast<void*>(this->value);
}

bool Boolean::getValue() {
  return this->value;
}