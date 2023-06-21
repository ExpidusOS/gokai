#include <gokai/view/display.h>
#include <stdexcept>

using namespace Gokai::View;

Display::Display(Gokai::ObjectArguments arguments) : Object(arguments) {}

std::string Display::getName() {
  return "";
}

std::string Display::getModel() {
  return "";
}

bool Display::isHDR() {
  return false;
}

glm::uvec2 Display::getPhysicalSize() {
  return glm::uvec2(0, 0);
}

std::list<DisplayMode> Display::getModes() {
  return std::list<DisplayMode>();
}

void Display::setMode(DisplayMode mode) {
  throw std::runtime_error("Display::setMode is not implemented.");
}