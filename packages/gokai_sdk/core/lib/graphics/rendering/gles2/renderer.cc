#include <gokai/graphics/rendering/gles2/renderer.h>

using namespace Gokai::Graphics::Rendering::GLES2;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}