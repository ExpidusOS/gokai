#include <gokai/plugin-module.h>
#include <assert.h>
#include <stdexcept>

using namespace Gokai;

PluginModule::PluginModule(Gokai::ObjectArguments arguments) : Object(arguments), lib{nullptr} {
  if (arguments.has("path")) {
    this->lib = os::DynamicLibrary::open(std::any_cast<std::string>(arguments.get("path")));
  } else if (arguments.has("library")) {
    this->lib = std::any_cast<os::DynamicLibrary*>(arguments.get("library"));
  } else {
    throw std::runtime_error("Failed to load library for plugin module: library or path argument is not set.");
  }

  assert(this->lib != nullptr);

  if (!this->lib->providesSymbol(std::string("gokai_plugin_metadata"))) {
    throw std::runtime_error("Library does not provide \"gokai_plugin_metadata\" symbol");
  }

  this->metadata = *(this->lib->lookup<PluginMetadata*>(std::string("gokai_plugin_metadata")));

  if (!this->lib->providesSymbol(std::string("gokai_plugin_load"))) {
    throw std::runtime_error("Library does not provide \"gokai_plugin_load\" symbol");
  }

  auto load = this->lib->lookup<PluginLoadFunc>(std::string("gokai_plugin_load"));
  auto loaded_impl = load(Gokai::ObjectArguments({
    { "context", arguments.get("context") },
    { "logger", arguments.get("logger") },
    { "tag", this->metadata.name },
  }));

  if (loaded_impl == nullptr) {
    throw std::runtime_error("Failed to load plugin due to \"gokai_plugin_load\" returning a null pointer.");
  }

  this->impl = loaded_impl;
}

PluginModule::~PluginModule() {
  delete this->lib;
}

PluginMetadata PluginModule::getMetadata() {
  return this->metadata;
}

Gokai::Plugin* PluginModule::getImpl() {
  return this->impl;
}
