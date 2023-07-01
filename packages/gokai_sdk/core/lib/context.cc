#include <gokai/context.h>
#include <gokai/services/compositor.h>
#include <gokai/services/display-manager.h>
#include <gokai/services/package-manager.h>
#include <gokai/services/window-manager.h>
#include <spdlog/spdlog.h>
#include <assert.h>
#include <filesystem>
#include <stdexcept>
#include <stdlib.h>
#include <type_traits>

#define TAG "Gokai::Context"

using namespace Gokai;

ContextMode::ContextMode() : ContextMode(-1, "") {}

ContextMode::ContextMode(uint8_t id, std::string name) : id{id}, name{name} {}

bool ContextMode::operator==(ContextMode b) {
  return this->id == b.id && this->name.compare(b.name) == 0;
}

bool ContextMode::operator!=(ContextMode b) {
  return this->id != b.id || this->name.compare(b.name) != 0;
}

ContextMode ContextMode::fromValue(std::any value) {
  if (value.type() == typeid(int)) {
    auto num = std::any_cast<int>(value);

    for (auto value : ContextMode::values) {
      if (value != ContextMode::invalid) {
        if (value.id == num) return value;
      }
    }

    throw std::invalid_argument("Invalid value provided");
  } else if (value.type() == typeid(std::string)) {
    auto str = std::any_cast<std::string>(value);

    for (auto value : ContextMode::values) {
      if (value != ContextMode::invalid) {
        if (value.name.compare(str) == 0) return value;
      }
    }

    throw std::invalid_argument("Invalid value provided");
  }

  throw std::runtime_error("Value is not an integer or string");
}

const ContextMode ContextMode::invalid = ContextMode();
const ContextMode ContextMode::client = ContextMode(0, "client");
const ContextMode ContextMode::compositor = ContextMode(1, "compositor");
const ContextMode ContextMode::values[3] = {
  ContextMode::invalid,
  ContextMode::client,
  ContextMode::compositor,
};

Context::Context(ObjectArguments arguments) : Loggable(TAG, arguments), method_codec{arguments} {
  auto manifest = this->getManifest();
  this->logger->debug("Manifest loaded with ID {}", manifest.id);

  auto find = manifest.defaults.find("Gokai::Context::mode");

  if (find != manifest.defaults.end()) {
    this->mode = ContextMode::fromValue(find->second);
  } else {
    this->mode = arguments.has("mode") ? ContextMode::fromValue(arguments.get("mode"))
      : ContextMode::client;
  }

  auto env = getenv("GOKAI_CONTEXT_MODE");
  if (env != nullptr) {
    this->mode = ContextMode::fromValue(std::string(env));
  }

  find = manifest.overrides.find("Gokai::Context::mode");
  if (find != manifest.overrides.end()) {
    this->mode = ContextMode::fromValue(find->second);
  }

  this->logger->debug("Creating context in mode \"{}\"", this->mode.name);

  this->loop = reinterpret_cast<uv_loop_t*>(malloc(sizeof (uv_loop_t)));
  assert(this->loop != nullptr);
  uv_loop_init(this->loop);

  this->onChannelReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);
    if (call.method.compare("getService") == 0) {
      auto name = std::any_cast<std::string>(call.arguments);
      auto service = this->getSystemService(name);
      if (service == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Service \"{}\" does not exist", name), std::make_any<void*>(nullptr));
      }

      auto service_channel = service->getServiceChannel();
      if (service_channel == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Service \"{}\" does not have a service channel", name), std::make_any<void*>(nullptr));
      }
      return this->method_codec.encodeSuccessEnvelope(service_channel->getName());
    }

    if (call.method.compare("getServiceNames") == 0) {
      auto names = this->getSystemServiceNames();
      std::list<std::any> list;
      for (auto name : names) list.push_back(name);
      return this->method_codec.encodeSuccessEnvelope(list);
    }

    if (call.method.compare("getMode") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->mode.name);
    }

    if (call.method.compare("getPackageName") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->getPackageName());
    }

    if (call.method.compare("getPackageDir") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->getPackageDir());
    }

    if (call.method.compare("getPackageConfigDir") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->getPackageConfigDir());
    }

    if (call.method.compare("getPackageDataDir") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->getPackageDataDir());
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });
}

Context::~Context() {
  uv_loop_close(this->loop);
}

uv_loop_t* Context::getLoop() {
  return this->loop;
}

std::vector<uint8_t> Context::channelReceive(xg::Guid engine_id, std::vector<uint8_t> message) {
  for (auto func : this->onChannelReceive) {
    auto result = func(engine_id, message);
    if (result.size() > 0) {
      return result;
    }
  }
  return std::vector<uint8_t>();
}

ApplicationManifest Context::getManifest() {
  return ApplicationManifest(YAML::LoadFile(std::filesystem::path(this->getPackageDir()) / "data" / "manifest.yaml"));
}

ContextMode Context::getMode() {
  return this->mode;
}

template<class T> T* Context::getSystemService() {
  if (std::is_same<T, Services::Compositor>::value) {
    return getSystemService(Services::Compositor::SERVICE_NAME);
  }

  if (std::is_same<T, Services::DisplayManager>::value) {
    return getSystemService(Services::DisplayManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::PackageManager>::value) {
    return getSystemService(Services::PackageManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::WindowManager>::value) {
    return getSystemService(Services::WindowManager::SERVICE_NAME);
  }
  return nullptr;
}

std::list<std::string> Context::getSystemServiceNames() {
  std::list<std::string> list;
  for (const auto& item : this->services) list.push_back(item.first);
  return list;
}

Service* Context::getSystemService(std::string serviceName) {
  auto pair = this->services.find(serviceName);
  if (pair != this->services.end()) return pair->second;
  return nullptr;
}

std::string Context::getPackageName() {
  return std::string();
}

std::string Context::getPackageDir() {
  return std::string();
}

std::string Context::getPackageConfigDir() {
  return std::filesystem::path(this->getPackageDir()) / "config";
}

std::string Context::getPackageDataDir() {
  return std::filesystem::path(this->getPackageDir()) / "data";
}