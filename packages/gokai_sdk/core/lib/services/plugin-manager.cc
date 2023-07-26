#include <gokai/services/plugin-manager.h>
#include <filesystem>

#define TAG "Gokai::Services::PluginManager"

using namespace Gokai::Services;

PluginManager::PluginManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getKeys") == 0) {
      std::list<std::any> list;
      for (const auto& entry : this->map) list.push_back(entry.first);
      return this->method_codec.encodeSuccessEnvelope(list);
    }

    if (call.method.compare("getMetadata") == 0) {
      auto find = this->map.find(std::any_cast<std::string>(call.arguments));
      if (find == this->map.end()) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Cannot find plugin"), std::make_any<void*>(nullptr));
      }

      auto metadata = find->second->getMetadata();

      std::map<std::string, std::any> map;
      map["name"] = metadata.name;
      map["description"] = metadata.description;
      map["homepage"] = metadata.homepage;
      map["license"] = metadata.license;
      return this->method_codec.encodeSuccessEnvelope(map);
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });

  this->paths.push_back(std::filesystem::path(this->context->getPackageDir()) / "lib" / "gokai" / "plugins");
  this->reload();
}

std::shared_ptr<Gokai::ServiceChannel> PluginManager::getServiceChannel() {
  return this->service_channel;
}

void PluginManager::reload() {
  for (const auto& entry : this->map) {
    if (!std::filesystem::exists(entry.first)) {
      delete entry.second;
      this->map.erase(entry.first);
    }
  }

  for (const auto& plugin_dir : this->paths) {
    try {
      this->logger->debug("Loading plugins from {}", plugin_dir);
      for (const auto& entry : std::filesystem::directory_iterator(plugin_dir)) {
        try {
          this->map[entry.path()] = new Gokai::PluginModule(Gokai::ObjectArguments({
            { "context", this->context },
            { "logger", this->getLogger() },
            { "path", entry.path().string() },
          }));
        } catch (const std::exception& ex) {
          this->logger->error("Failed to load plugin {}: {}", entry.path().string(), ex.what());
        }
      }
    } catch (const std::exception& ex) {
      this->logger->error("Failed to load plugins from {}: {}", plugin_dir, ex.what());
    }
  }
}

const std::string PluginManager::SERVICE_NAME = "PluginManager";
