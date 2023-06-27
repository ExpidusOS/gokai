#include <gokai/input/touch.h>

#define TAG "Gokai::Input::Touch"

using namespace Gokai::Input;

Touch::Touch(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}