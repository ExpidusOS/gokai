#include <gdk/gdk.h>
#include <gdk/gdkwayland.h>
#include <gdk/gdkx.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/context.h>
#include <gokai/values/int.h>
#include <gokai/values/pointer.h>
#include <gokai/values/string.h>
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

ContextDisplayBackend ContextDisplayBackend::fromValue(Value* value) {
  if (dynamic_cast<Values::Integer*>(value)) {
    auto num = dynamic_cast<Values::Integer*>(value);

    for (auto value : ContextDisplayBackend::values) {
      if (value.id == num->getValue()) return value;
    }

    throw std::invalid_argument("Invalid value provided");
  } else if (dynamic_cast<Values::String*>(value)) {
    auto str = dynamic_cast<Values::String*>(value);

    for (auto value : ContextDisplayBackend::values) {
      if (value.name.compare(str->getValue()) == 0) return value;
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
  this->display_backend = arguments.has("display-backend") ? ContextDisplayBackend::fromValue(arguments.getPointed("display-backend"))
    : (this->getMode() == Gokai::ContextMode::compositor ? ContextDisplayBackend::wayland : ContextDisplayBackend::try_auto);

  AsMetadata* metadata = AS_METADATA(arguments.get("metadata").toPointer());
  assert(metadata != nullptr);

  this->metadata = AS_METADATA(g_object_ref(G_OBJECT(metadata)));
  assert(this->metadata != nullptr);

  AsFormatStyle style = as_metadata_get_format_style(this->metadata);
  if (style != AS_FORMAT_STYLE_METAINFO) {
    throw std::invalid_argument("Expected metainfo AppStream style metadata.");
  }

  assert(xdgInitHandle(&this->xdg_handle) != nullptr);

  this->services = std::map<std::string, Gokai::Service*>();

  this->package_manager = new Services::PackageManager(Gokai::ObjectArguments({
    { "context", Gokai::Values::Pointer(this) },
  }));

  if (this->getMode() == Gokai::ContextMode::client) {
    GdkDisplayManager* display_manager = gdk_display_manager_get();

    if (this->getDisplayBackend() == ContextDisplayBackend::try_auto) {
      GdkDisplay* display = gdk_display_manager_get_default_display(display_manager);

      if (GDK_IS_X11_DISPLAY(display)) {
      } else if (GDK_IS_WAYLAND_DISPLAY(display)) {
      } else {
        throw std::runtime_error("Unsupported GDK display.");
      }
    } else if (this->getDisplayBackend() == ContextDisplayBackend::x11) {
    } else if (this->getDisplayBackend() == ContextDisplayBackend::wayland) {
    } else {
      g_object_unref(display_manager);
      throw std::runtime_error("Unsupported display backend");
    }

    g_object_unref(display_manager);
  } else if (this->getMode() == Gokai::ContextMode::compositor) {
    if (this->getDisplayBackend() == ContextDisplayBackend::wayland) {
      this->services[Gokai::Services::Compositor::SERVICE_NAME] = new Services::Wayland::Server::Compositor(Gokai::ObjectArguments({
        { "context", Gokai::Values::Pointer(this) },
      }));

      this->services[Gokai::Services::DisplayManager::SERVICE_NAME] = new Services::Wayland::Server::DisplayManager(Gokai::ObjectArguments({
        { "context", Gokai::Values::Pointer(this) },
      }));

      this->services[Gokai::Services::WindowManager::SERVICE_NAME] = new Services::Wayland::Server::WindowManager(Gokai::ObjectArguments({
        { "context", Gokai::Values::Pointer(this) },
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

  g_object_unref(G_OBJECT(this->metadata));
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