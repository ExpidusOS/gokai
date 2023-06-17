#include <gokai/services/package-manager.h>

using namespace Gokai::Services;

PackageManager::PackageManager(Gokai::ObjectArguments arguments) : Service(arguments) {}

const std::string PackageManager::SERVICE_NAME = "PackageManager";