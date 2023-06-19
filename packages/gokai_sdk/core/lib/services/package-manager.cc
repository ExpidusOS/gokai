#include <gokai/services/package-manager.h>

#define TAG "Gokai::Services::PackageManager"

using namespace Gokai::Services;

PackageManager::PackageManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

const std::string PackageManager::SERVICE_NAME = "PackageManager";