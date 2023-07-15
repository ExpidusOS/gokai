#include <gokai/view/window.h>

using namespace Gokai::View;

Window::Window(Gokai::ObjectArguments arguments) : Object(arguments) {
  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  } else {
    this->id = xg::newGuid();
  }
}

std::string Window::getDisplayName() {
  return std::string();
}

xg::Guid Window::getId() {
  return this->id;
}

URect Window::getRect() {
  return this->rect;
}

void Window::setRect(URect rect) {
  this->rect = rect;
}

bool Window::hasDecorations() {
  return false;
}

bool Window::hasTexture() {
  return false;
}

int64_t Window::getTextureId() {
  return 0;
}

std::shared_ptr<Gokai::Graphics::Texture> Window::getTexture() {
  return nullptr;
}

std::string Window::getTitle() {
  return "";
}