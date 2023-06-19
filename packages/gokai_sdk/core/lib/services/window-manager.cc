#include <gokai/services/window-manager.h>

#define TAG "Gokai::Services::WindowManager"

using namespace Gokai::Services;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

const std::string WindowManager::SERVICE_NAME = "WindowManager";