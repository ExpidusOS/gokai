#include <gokai/view/window.h>

using namespace Gokai::View;

Window::Window(Gokai::ObjectArguments arguments) : Object(arguments) {
  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  } else {
    this->id = xg::newGuid();
  }
}

bool Window::isToplevel() {
  return false;
}

std::string Window::getRole() {
  return "";
}

std::list<xg::Guid> Window::getChildrenAbove() {
  return std::list<xg::Guid>();
}

std::list<xg::Guid> Window::getChildrenBelow() {
  return std::list<xg::Guid>();
}

std::string Window::getDisplayName() {
  return std::string();
}

bool Window::isMapped() {
  return this->is_mapped;
}

void Window::setMapped(bool value) {
  this->is_mapped = value;
  for (const auto& func : this->onMapped) func();
}

bool Window::isActive() {
  return this->is_active;
}

void Window::setActive(bool value) {
  this->is_active = value;
  for (const auto& func : this->onActive) func();
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