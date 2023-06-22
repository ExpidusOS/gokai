#include <gokai/graphics/rendering/pixman/renderer.h>

using namespace Gokai::Graphics::Rendering::Pixman;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}