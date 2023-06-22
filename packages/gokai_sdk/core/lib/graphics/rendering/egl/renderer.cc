#include <gokai/graphics/rendering/egl/renderer.h>

using namespace Gokai::Graphics::Rendering::EGL;

Renderer::Renderer(EGLDisplay display, EGLContext context, Gokai::ObjectArguments arguments) : Gokai::Graphics::Rendering::GLES2::Renderer(arguments), display{display}, context{context} {
}

FlutterRendererConfig* Renderer::getConfig() {
  return nullptr;
}