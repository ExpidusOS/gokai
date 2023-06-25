#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/context.h>
#include <assert.h>
#include <basedir.h>
#include <filesystem>
#include <stdexcept>
#include <stdlib.h>

using namespace Gokai::Framework::os::Linux;

ContextDisplayBackend::ContextDisplayBackend() : ContextDisplayBackend(0, "try-auto") {}

ContextDisplayBackend::ContextDisplayBackend(uint8_t id, std::string name) {
  this->id = id;
  this->name = name;
}

bool ContextDisplayBackend::operator==(ContextDisplayBackend b) {
  return this->id == b.id && this->name.compare(b.name) == 0;
}

bool ContextDisplayBackend::operator!=(ContextDisplayBackend b) {
  return this->id != b.id || this->name.compare(b.name) != 0;
}

ContextDisplayBackend ContextDisplayBackend::fromValue(std::any value) {
  if (value.type() == typeid(int)) {
    auto num = std::any_cast<int>(value);

    for (auto value : ContextDisplayBackend::values) {
      if (value.id == num) return value;
    }

    throw std::invalid_argument("Invalid value provided");
  } else if (value.type() == typeid(std::string)) {
    auto str = std::any_cast<std::string>(value);

    for (auto value : ContextDisplayBackend::values) {
      if (value.name.compare(str) == 0) return value;
    }

    throw std::invalid_argument("Invalid value provided");
  }

  throw std::runtime_error("Value is not an integer or string");
}

const ContextDisplayBackend ContextDisplayBackend::try_auto = ContextDisplayBackend();
const ContextDisplayBackend ContextDisplayBackend::x11 = ContextDisplayBackend(1, "x11");
const ContextDisplayBackend ContextDisplayBackend::wayland = ContextDisplayBackend(2, "wayland");
const ContextDisplayBackend ContextDisplayBackend::values[3] = {
  ContextDisplayBackend::try_auto,
  ContextDisplayBackend::x11,
  ContextDisplayBackend::wayland,
};

Context::Context(Gokai::ObjectArguments arguments) : Gokai::Context(arguments) {
  auto manifest = this->getManifest();
  auto find = manifest.defaults.find("Gokai::Context::display-backend");

  if (find != manifest.defaults.end()) {
    this->display_backend = ContextDisplayBackend::fromValue(find->second);
  } else {
    this->display_backend = arguments.has("display-backend") ? ContextDisplayBackend::fromValue(arguments.get("display-backend"))
      : (this->getMode() == Gokai::ContextMode::compositor ? ContextDisplayBackend::wayland : ContextDisplayBackend::try_auto);
  }

  auto env = getenv("GOKAI_CONTEXT_DISPLAY_BACKEND");
  if (env != nullptr) {
    this->display_backend = ContextDisplayBackend::fromValue(std::string(env));
  }

  find = manifest.overrides.find("Gokai::Context::display-backend");
  if (find != manifest.overrides.end()) {
    this->display_backend = ContextDisplayBackend::fromValue(find->second);
  }

  this->logger->debug("Using the display backend: {}", this->display_backend.name);

  assert(xdgInitHandle(&this->xdg_handle) != nullptr);

  this->services = std::map<std::string, Gokai::Service*>();
  auto self = std::shared_ptr<Gokai::Context>(static_cast<Gokai::Context*>(this));

  this->engine_manager = new Gokai::Services::EngineManager(Gokai::ObjectArguments({
    { "context", self },
    { "logger", this->getLogger() },
  }));

  this->package_manager = new Services::PackageManager(Gokai::ObjectArguments({
    { "context", self },
    { "logger", this->getLogger() },
  }));

  if (this->getMode() == Gokai::ContextMode::client) {
    if (this->getDisplayBackend() == ContextDisplayBackend::try_auto) {
      // TODO: discover with environmental variables
    } else if (this->getDisplayBackend() == ContextDisplayBackend::x11) {
    } else if (this->getDisplayBackend() == ContextDisplayBackend::wayland) {
    } else {
      throw std::runtime_error("Unsupported display backend");
    }
  } else if (this->getMode() == Gokai::ContextMode::compositor) {
    if (this->getDisplayBackend() == ContextDisplayBackend::wayland) {
      auto compositor = new Services::Wayland::Server::Compositor(Gokai::ObjectArguments({
        { "context", self },
        { "logger", this->getLogger() },
      }));

      this->services[Gokai::Services::Compositor::SERVICE_NAME] = compositor;

      this->services[Gokai::Services::DisplayManager::SERVICE_NAME] = new Services::Wayland::Server::DisplayManager(Gokai::ObjectArguments({
        { "context", self },
        { "logger", this->getLogger() },
      }));

      this->services[Gokai::Services::WindowManager::SERVICE_NAME] = new Services::Wayland::Server::WindowManager(Gokai::ObjectArguments({
        { "context", self },
        { "logger", this->getLogger() },
      }));

      compositor->start();
    } else {
      throw std::runtime_error("Unsupported display backend");
    }
  } else {
    throw std::runtime_error("Unsupported context mode");
  }
}

Context::~Context() {
  delete this->package_manager;
  delete this->engine_manager;

  for (auto service : this->services) delete service.second;

  xdgWipeHandle(&this->xdg_handle);
}

ContextDisplayBackend Context::getDisplayBackend() {
  return this->display_backend;
}

Gokai::Service* Context::getSystemService(std::string serviceName) {
  if (serviceName.compare(Gokai::Services::PackageManager::SERVICE_NAME) == 0) {
    return this->package_manager;
  }

  if (serviceName.compare(Gokai::Services::EngineManager::SERVICE_NAME) == 0) {
    return this->engine_manager;
  }

  auto pair = this->services.find(serviceName);
  if (pair != this->services.end()) return pair->second;
  return nullptr;
}

std::string Context::getPackageDir() {
  struct stat sb;
  lstat("/proc/self/exe", &sb);

  size_t linksize = sb.st_size + 1;
  if (linksize == 1) linksize = PATH_MAX;

  char* link_path = reinterpret_cast<char*>(malloc(linksize));
  assert(link_path != nullptr);

  size_t read_size = readlink("/proc/self/exe", link_path, linksize);
  if (read_size < 0) {
    throw std::runtime_error("Failed to readlink /proc/self/exe");
  }
  return std::filesystem::path(link_path).parent_path();
}

std::string Context::getPackageConfigDir() {
  return std::filesystem::path(xdgConfigHome(&this->xdg_handle)) / this->getPackageName();
}