#include <gokai/values/int.h>

using namespace Gokai::Values;

Integer::Integer(int value) {
  this->value = value;
}

void* Integer::toPointer() {
  return reinterpret_cast<void*>(this->value);
}

int Integer::getValue() {
  return this->value;
}