#include <gokai/context.h>
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

Context::Context(ObjectArguments arguments) : Loggable(TAG, arguments) {
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
}

Context::~Context() {
  // FIXME: causes segfault but we need to clean up
  // TODO: make this only call for the main instance
  //uv_loop_close(this->loop);
  //free(this->loop);
}

uv_loop_t* Context::getLoop() {
  return this->loop;
}

ApplicationManifest Context::getManifest() {
  return ApplicationManifest(YAML::LoadFile(std::filesystem::path(this->getPackageDir()) / "data" / "manifest.yaml"));
}

ContextMode Context::getMode() {
  return this->mode;
}

template<class T> T* Context::getSystemService() {
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

Service* Context::getSystemService(std::string serviceName) {
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