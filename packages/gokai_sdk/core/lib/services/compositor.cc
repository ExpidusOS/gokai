#include <gokai/services/compositor.h>

using namespace Gokai::Services;

Compositor::Compositor(Gokai::ObjectArguments arguments) : Service(arguments) {}

const std::string Compositor::SERVICE_NAME = "Compositor";