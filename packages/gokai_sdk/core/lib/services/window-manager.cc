#include <gokai/services/window-manager.h>

using namespace Gokai::Services;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Service(arguments) {}

const std::string WindowManager::SERVICE_NAME = "WindowManager";