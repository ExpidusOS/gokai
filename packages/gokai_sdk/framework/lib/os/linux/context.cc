#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/display-manager.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>
#include <gokai/framework/os/linux/context.h>
#include <gokai/values/int.h>
#include <gokai/values/pointer.h>
#include <assert.h>
#include <basedir.h>
#include <filesystem>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux;

Context::Context(Gokai::ObjectArguments arguments) : Gokai::Context(arguments) {
  this->type = GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_AUTO;

  if (arguments.has("type")) {
    this->type = static_cast<ContextType>(reinterpret_cast<Gokai::Values::Integer*>(arguments.getPointed("type"))->getValue());
  }

  if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_AUTO) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_AUTO) {
    // TODO: determine based on GDK and then recalculate the type property based on it.
  }

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

  if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_CLIENT) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_CLIENT) {
    if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_WAYLAND) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_WAYLAND) {
    } else if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_X11) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_X11) {
    } else {
      throw std::invalid_argument("Unknown or unsupported mode of operation");
    }
  } else if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_COMPOSITOR) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_COMPOSITOR) {
    if ((this->type & GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_WAYLAND) == GOKAI_FRAMEWORK_LINUX_CONTEXT_TYPE_WAYLAND) {
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
      throw std::invalid_argument("Unknown or unsupported mode of operation");
    }
  } else {
    throw std::invalid_argument("Context was not supplied with the correct display mode");
  }
}

Context::~Context() {
  delete this->package_manager;
  for (auto service : this->services) delete service.second;

  g_object_unref(G_OBJECT(this->metadata));
  xdgWipeHandle(&this->xdg_handle);
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