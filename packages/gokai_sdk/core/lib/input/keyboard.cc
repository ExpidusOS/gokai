#include <gokai/input/keyboard.h>

#define TAG "Gokai::Input::Keyboard"

using namespace Gokai::Input;

Keyboard::Keyboard(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}