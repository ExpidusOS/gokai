#include <gokai/input/pointer.h>

#define TAG "Gokai::Input::Pointer"

using namespace Gokai::Input;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}