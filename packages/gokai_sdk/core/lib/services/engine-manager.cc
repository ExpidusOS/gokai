#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::EngineManager"

using namespace Gokai::Services;

EngineManager::EngineManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->engines = new std::map<xg::Guid, Gokai::Flutter::Engine>();
  this->logger->debug("Service created");
}

EngineManager::~EngineManager() {
  delete this->engines;
}

Gokai::Flutter::Engine* EngineManager::create(Gokai::Graphics::Renderer* renderer) {
  xg::Guid id;
  return &this->engines->emplace(id, Gokai::Flutter::Engine(Gokai::ObjectArguments({
    { "id", id },
    { "logger", this->getLogger() },
    { "context", this->context },
    { "renderer", renderer },
  }))).first->second;
}

void EngineManager::destroy(xg::Guid id) {
  this->engines->erase(id);
}

const std::string EngineManager::SERVICE_NAME = "EngineManager";