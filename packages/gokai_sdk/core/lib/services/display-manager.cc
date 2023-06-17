#include <gokai/services/display-manager.h>

using namespace Gokai::Services;

DisplayManager::DisplayManager(Gokai::Context* context) : Service(context) {}

const std::string DisplayManager::SERVICE_NAME = "DisplayManager";