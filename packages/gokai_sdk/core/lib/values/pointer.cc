#include <gokai/values/pointer.h>

using namespace Gokai::Values;

Pointer::Pointer(void* ptr) {
  this->value = ptr;
}

void* Pointer::toPointer() {
  return this->value;
}