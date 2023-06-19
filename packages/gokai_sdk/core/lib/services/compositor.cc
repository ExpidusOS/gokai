#include <gokai/services/compositor.h>

#define TAG "Gokai::Services::Compositor"

using namespace Gokai::Services;

Compositor::Compositor(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

const std::string Compositor::SERVICE_NAME = "Compositor";