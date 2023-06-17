#include <gokai/framework/os/linux/context.h>
#include <assert.h>
#include <basedir.h>
#include <filesystem>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux;

Context::Context(Gokai::ObjectArguments arguments) : Gokai::Context(arguments) {
  AsMetadata* metadata = AS_METADATA(arguments.get("metadata").toPointer());
  assert(metadata != nullptr);

  this->metadata = AS_METADATA(g_object_ref(G_OBJECT(metadata)));
  assert(this->metadata != nullptr);

  AsFormatStyle style = as_metadata_get_format_style(this->metadata);
  if (style != AS_FORMAT_STYLE_METAINFO) {
    throw std::invalid_argument("Expected metainfo AppStream style metadata.");
  }

  assert(xdgInitHandle(&this->xdg_handle) != nullptr);
}

Context::~Context() {
  g_object_unref(G_OBJECT(this->metadata));
  xdgWipeHandle(&this->xdg_handle);
}

Gokai::Service* Context::getSystemService(std::string serviceName) {
  return nullptr;
}

std::string Context::getPackageName() {
  AsComponent* comp = as_metadata_get_component(this->metadata);
  assert(comp != nullptr);
  return std::string(as_component_get_pkgname(comp));
}

std::string Context::getPackageDir() {
  return std::filesystem::path(xdgConfigHome(&this->xdg_handle)) / this->getPackageName();
}