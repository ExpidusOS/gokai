#include <gokai/services/texture-manager.h>

#define TAG "Gokai::Services::TextureManager"

using namespace Gokai::Services;

TextureManager::TextureManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
  this->next_id = 0;
}

int64_t TextureManager::allocate() {
  int64_t id = this->next_id++;
  this->logger->debug("Registering texture {}", id);
  // TODO: register on all
  return id;
}

void TextureManager::unregister(int64_t id) {
  // TODO: unregister on all
}

const std::string TextureManager::SERVICE_NAME = "TextureManager";