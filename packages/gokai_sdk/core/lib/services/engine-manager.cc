#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::EngineManager"

using namespace Gokai::Services;

EngineManager::EngineManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
}

EngineManager::~EngineManager() {}

std::shared_ptr<Gokai::Flutter::Engine> EngineManager::create(Gokai::Graphics::Renderer* renderer) {
  auto id = xg::newGuid();

  auto engine = std::shared_ptr<Gokai::Flutter::Engine>(new Gokai::Flutter::Engine(Gokai::ObjectArguments({
    { "id", id },
    { "logger", this->getLogger() },
    { "context", this->context },
    { "renderer", renderer },
  })));

  this->engines[id] = engine.get();
  return engine;
}

void EngineManager::destroy(xg::Guid id) {
  this->engines.erase(id);
}

const std::string EngineManager::SERVICE_NAME = "EngineManager";