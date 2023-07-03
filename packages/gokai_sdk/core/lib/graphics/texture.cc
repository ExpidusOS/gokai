#include <gokai/graphics/texture.h>

using namespace Gokai::Graphics;

Texture::Texture(Gokai::ObjectArguments arguments) : Object(arguments) {}

bool Texture::frame(size_t width, size_t height, Gokai::Flutter::Texture* out) {
  return false;
}