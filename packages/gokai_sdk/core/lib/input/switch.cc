#include <gokai/input/switch.h>

#define TAG "Gokai::Input::Switch"

using namespace Gokai::Input;

Switch::Switch(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}
