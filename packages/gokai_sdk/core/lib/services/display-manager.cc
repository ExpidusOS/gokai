#include <gokai/services/display-manager.h>

using namespace Gokai::Services;

DisplayManager::DisplayManager(Gokai::ObjectArguments arguments) : Service(arguments) {}

const std::string DisplayManager::SERVICE_NAME = "DisplayManager";