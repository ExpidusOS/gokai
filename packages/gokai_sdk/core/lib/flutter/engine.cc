#include <gokai/flutter/engine.h>
#include <filesystem>
#include <stdexcept>

#define TAG "Gokai::Flutter::Engine"

using namespace Gokai::Flutter;

Engine::Engine(Gokai::ObjectArguments arguments) : Gokai::Loggable(TAG, arguments) {
  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  }

  this->renderer = std::any_cast<Gokai::Graphics::Renderer*>(arguments.get("renderer"));
  this->context = std::any_cast<Gokai::Context*>(arguments.get("context"));

  auto path = std::filesystem::path(this->context->getPackageDir());

  this->args = {};
  this->args.struct_size = sizeof (FlutterProjectArgs);
  this->args.assets_path = strdup((path / "data" / "flutter_assets").c_str());
  this->args.icu_data_path = strdup((path / "data" / "icudtl.dat").c_str());

  if (FlutterEngineRunsAOTCompiledDartCode()) {
    FlutterEngineAOTDataSource src = {
      .type = kFlutterEngineAOTDataSourceTypeElfPath,
      .elf_path = strdup((path / "lib" / "libapp.so").c_str()),
    };

    auto result = FlutterEngineCreateAOTData(&src, &this->args.aot_data);
    if (result != kSuccess) {
      throw std::runtime_error(fmt::format("Failed to create AOT data: {}", result));
    }
  }

  auto result = FlutterEngineInitialize(
    FLUTTER_ENGINE_VERSION,
    this->renderer->getConfig(),
    &this->args,
    this,
    &this->value
  );
  if (result != kSuccess) {
    throw std::runtime_error(fmt::format("Failed to initialize the engine: {}", result));
  }

  this->logger->debug("Flutter engine {} has initialized", this->id.str());

  if ((result = FlutterEngineRunInitialized(this->value)) != kSuccess) {
    throw std::runtime_error(fmt::format("Failed to start the engine: {}", result));
  }
}

xg::Guid Engine::getId() {
  return this->id;
}

Gokai::Graphics::Renderer* Engine::getRenderer() {
  return this->renderer;
}

Gokai::Context* Engine::getContext() {
  return this->context;
}