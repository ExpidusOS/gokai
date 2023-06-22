#include <gokai/flutter/engine.h>

#define TAG "Gokai::Flutter::Engine"

using namespace Gokai::Flutter;

Engine::Engine(Gokai::ObjectArguments arguments) : Gokai::Loggable(TAG, arguments) {
  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  }

  this->renderer = std::any_cast<Gokai::Graphics::Renderer*>(arguments.get("renderer"));
  this->context = std::any_cast<Gokai::Context*>(arguments.get("context"));
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