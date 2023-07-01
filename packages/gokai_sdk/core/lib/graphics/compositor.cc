#include <gokai/graphics/compositor.h>

#define TAG "Gokai::Graphics::Compositor"

using namespace Gokai::Graphics;

Compositor::Compositor(Gokai::ObjectArguments arguments) : Loggable(TAG, arguments) {}

FlutterCompositor* Compositor::get() {
  return nullptr;
}