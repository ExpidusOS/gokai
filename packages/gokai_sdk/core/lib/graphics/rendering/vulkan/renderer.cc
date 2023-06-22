#include <gokai/graphics/rendering/vulkan/renderer.h>

using namespace Gokai::Graphics::Rendering::Vulkan;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Gokai::Graphics::Renderer(arguments) {}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}