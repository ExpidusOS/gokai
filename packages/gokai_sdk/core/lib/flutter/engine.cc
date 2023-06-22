#include <gokai/flutter/engine.h>

#define TAG "Gokai::Flutter::Engine"

using namespace Gokai::Flutter;

Engine::Engine(Gokai::ObjectArguments arguments) : Gokai::Loggable(TAG, arguments) {}

Gokai::Graphics::Renderer* Engine::getRenderer() {
  return this->renderer;
}