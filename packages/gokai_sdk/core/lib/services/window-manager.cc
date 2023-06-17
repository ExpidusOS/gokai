#include <gokai/services/window-manager.h>

using namespace Gokai::Services;

WindowManager::WindowManager(Gokai::Context* context) : Service(context) {}

const std::string WindowManager::SERVICE_NAME = "WindowManager";