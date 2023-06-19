#include <gokai/value.h>

using namespace Gokai;

template<typename T> Value<T>::Value(T value) {
  this->value = value;
}

template<typename T> bool Value<T>::isSet() {
  return this->toPointer() != nullptr;
}

template<typename T> bool Value<T>::isEmpty() {
  return this->toPointer() != nullptr;
}

template<typename T> void* Value<T>::toPointer() {
  return &this->value;
}

template<typename T> T Value<T>::getValue() {
  return this->value;
}