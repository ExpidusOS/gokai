#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/context.h>
#include <assert.h>
#include <basedir.h>
#include <filesystem>
#include <stdexcept>

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
  this->display_backend = arguments.has("display-backend") ? ContextDisplayBackend::fromValue(arguments.get("display-backend"))
    : (this->getMode() == Gokai::ContextMode::compositor ? ContextDisplayBackend::wayland : ContextDisplayBackend::try_auto);

#if 0
  AsMetadata* metadata = AS_METADATA(std::any_cast<void*>(arguments.get("metadata")));
  assert(metadata != nullptr);

  this->metadata = AS_METADATA(g_object_ref(G_OBJECT(metadata)));
  assert(this->metadata != nullptr);

  AsFormatStyle style = as_metadata_get_format_style(this->metadata);
  if (style != AS_FORMAT_STYLE_METAINFO) {
    throw std::invalid_argument("Expected metainfo AppStream style metadata.");
  }
#endif

  assert(xdgInitHandle(&this->xdg_handle) != nullptr);

  this->services = std::map<std::string, Gokai::Service*>();

  this->package_manager = new Services::PackageManager(Gokai::ObjectArguments({
    { "context", static_cast<Gokai::Context*>(this) },
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
      this->services[Gokai::Services::Compositor::SERVICE_NAME] = new Services::Wayland::Server::Compositor(Gokai::ObjectArguments({
        { "context", static_cast<Gokai::Context*>(this) },
      }));

      this->services[Gokai::Services::DisplayManager::SERVICE_NAME] = new Services::Wayland::Server::DisplayManager(Gokai::ObjectArguments({
        { "context", static_cast<Gokai::Context*>(this) },
      }));

      this->services[Gokai::Services::WindowManager::SERVICE_NAME] = new Services::Wayland::Server::WindowManager(Gokai::ObjectArguments({
        { "context", static_cast<Gokai::Context*>(this) },
      }));
    } else {
      throw std::runtime_error("Unsupported display backend");
    }
  } else {
    throw std::runtime_error("Unsupported context mode");
  }
}

Context::~Context() {
  delete this->package_manager;
  for (auto service : this->services) delete service.second;

  // g_object_unref(G_OBJECT(this->metadata));
  xdgWipeHandle(&this->xdg_handle);
}

ContextDisplayBackend Context::getDisplayBackend() {
  return this->display_backend;
}

Gokai::Service* Context::getSystemService(std::string serviceName) {
  if (serviceName.compare(Gokai::Services::PackageManager::SERVICE_NAME) == 0) {
    return this->package_manager;
  }

  auto pair = this->services.find(serviceName);
  if (pair != this->services.end()) return pair->second;
  return nullptr;
}

std::string Context::getPackageName() {
  AsComponent* comp = as_metadata_get_component(this->metadata);
  assert(comp != nullptr);
  return std::string(as_component_get_pkgname(comp));
}

std::string Context::getPackageConfigDir() {
  return std::filesystem::path(xdgConfigHome(&this->xdg_handle)) / this->getPackageName();
}

std::string Context::getPackageDataDir() {
  return std::filesystem::path(xdgDataHome(&this->xdg_handle)) / this->getPackageName();
}