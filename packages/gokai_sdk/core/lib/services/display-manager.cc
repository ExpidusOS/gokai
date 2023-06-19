#include <gokai/services/display-manager.h>

#define TAG "Gokai::Services::DisplayManager"

using namespace Gokai::Services;

DisplayManager::DisplayManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

const std::string DisplayManager::SERVICE_NAME = "DisplayManager";