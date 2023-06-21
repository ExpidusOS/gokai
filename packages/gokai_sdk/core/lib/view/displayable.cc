#include <gokai/view/displayable.h>

using namespace Gokai::View;

Displayable::Displayable(Gokai::ObjectArguments arguments) : Object(arguments) {}

bool Displayable::resize(glm::uvec2 size) {
  for (auto func : this->onResize) {
    if (!func(size)) return false;
  }
  return true;
}

bool Displayable::frame(glm::uvec2 size) {
  for (auto func : this->onFrame) {
    if (!func(size)) return false;
  }
  return true;
}

URect Displayable::getRect() {
  return URect();
}