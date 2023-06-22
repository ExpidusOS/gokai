#include <gokai/graphics/renderer.h>

#define TAG "Gokai::Graphics::Renderer"

using namespace Gokai::Graphics;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments) {}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}

void Renderer::render(Gokai::View::Displayable& target) {}