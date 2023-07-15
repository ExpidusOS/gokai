#include <gokai/services/engine-manager.h>
#include <gokai/services/texture-manager.h>

#define TAG "Gokai::Services::TextureManager"

using namespace Gokai::Services;

TextureManager::TextureManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments) {
  this->logger->debug("Service created");
  this->next_id = 1;
}

int64_t TextureManager::allocate(std::shared_ptr<Gokai::Graphics::Texture> texture) {
  int64_t id = this->next_id++;
  this->logger->debug("Registering texture {}", id);
  this->map[id] = texture;

  auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
  for (const auto& engine_id : engine_manager->getIds()) {
    auto engine = engine_manager->get(engine_id);
    if (engine == nullptr) continue;

    auto result = FlutterEngineRegisterExternalTexture(engine->getValue(), id);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to register the texture");
    }

    result = FlutterEngineMarkExternalTextureFrameAvailable(engine->getValue(), id);
    if (result != kSuccess) {
      throw std::runtime_error("Failed to make the texture available");
    }
  }
  return id;
}

std::shared_ptr<Gokai::Graphics::Texture> TextureManager::get(int64_t id) {
  auto find = this->map.find(id);
  if (find == this->map.end()) return nullptr;
  return find->second;
}

void TextureManager::unregister(int64_t id) {
  auto find = this->map.find(id);
  if (find != this->map.end()) {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    for (const auto& engine_id : engine_manager->getIds()) {
      auto engine = engine_manager->get(engine_id);
      if (engine == nullptr) continue;

      auto result = FlutterEngineUnregisterExternalTexture(engine->getValue(), id);
      if (result != kSuccess) {
        throw std::runtime_error("Failed to register the texture");
      }
    }
    this->map.erase(find);
  }
}

const std::string TextureManager::SERVICE_NAME = "TextureManager";