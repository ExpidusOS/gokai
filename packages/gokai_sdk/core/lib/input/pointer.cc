#include <fmt/format.h>
#include <gokai/input/pointer.h>
#include <stdexcept>

#define TAG "Gokai::Input::Pointer"

using namespace Gokai::Input;

Pointer::Pointer(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}

Pointer::Event::Event(std::any any) : Event(std::any_cast<std::map<std::string, std::any>>(any)) {}

Pointer::Event::Event(std::map<std::string, std::any> map) {
  auto type = std::any_cast<std::string>(map["type"]);
  if (type.compare("hover") == 0) {
    this->type = Pointer::EventType::hover;
  } else if (type.compare("button") == 0) {
    this->type = Pointer::EventType::button;
  } else if (type.compare("leave") == 0) {
    this->type = Pointer::EventType::leave;
  } else {
    throw std::runtime_error(fmt::format("Invalid type {}", type));
  }

  this->window_id = xg::Guid(std::any_cast<std::string>(map["windowId"]));

  this->pos = glm::vec2(
    std::any_cast<double>(map["x"]),
    std::any_cast<double>(map["y"])
  );

  this->button = std::any_cast<int>(map["button"]);
  this->is_released = std::any_cast<bool>(map["isReleased"]);
}
