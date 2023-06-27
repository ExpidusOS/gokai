#include <gokai/services/input-manager.h>

#define TAG "Gokai::Services::InputManager"

using namespace Gokai::Services;

InputManager::InputManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

std::list<std::string> InputManager::getNames() {
  return std::list<std::string>();
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(std::string name) {
  return nullptr;
}

const std::string InputManager::SERVICE_NAME = "InputManager";