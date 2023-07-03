#include <gokai/services/texture-manager.h>

#define TAG "Gokai::Services::TextureManager"

using namespace Gokai::Services;

TextureManager::TextureManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
  this->next_id = 0;
}

int64_t TextureManager::allocate(std::shared_ptr<Gokai::Graphics::Texture> texture) {
  int64_t id = this->next_id++;
  this->logger->debug("Registering texture {}", id);
  this->map[id] = texture;
  return id;
}

std::shared_ptr<Gokai::Graphics::Texture> TextureManager::get(int64_t id) {
  auto find = this->map.find(id);
  if (find == this->map.end()) return nullptr;
  return find->second;
}

void TextureManager::unregister(int64_t id) {
  this->map.erase(id);
}

const std::string TextureManager::SERVICE_NAME = "TextureManager";