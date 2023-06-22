#include <gokai/graphics/renderer.h>

#define TAG "Gokai::Graphics::Renderer"

using namespace Gokai::Graphics;

Renderer::Renderer(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments) {}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}

glm::uvec2 Renderer::getSize() {
  return glm::uvec2();
}

void Renderer::resize(glm::uvec2 size) {}
void Renderer::render(Gokai::View::Displayable& target) {}