#include <fmt/core.h>
#include <gokai/input/touch.h>
#include <stdexcept>

#define TAG "Gokai::Input::Touch"

using namespace Gokai::Input;

Touch::Touch(Gokai::ObjectArguments arguments) : Base(arguments), Loggable(TAG, arguments) {
  this->logger->debug("New input {}", this->getName());
}

Touch::Event::Event(std::any any) : Event(std::any_cast<std::map<std::string, std::any>>(any)) {}

Touch::Event::Event(std::map<std::string, std::any> map) {
  auto type = std::any_cast<std::string>(map["type"]);
  if (type.compare("up") == 0) {
    this->type = Touch::EventType::up;
  } else if (type.compare("down") == 0) {
    this->type = Touch::EventType::down;
  } else if (type.compare("motion") == 0) {
    this->type = Touch::EventType::motion;
  } else if (type.compare("cancel") == 0) {
    this->type = Touch::EventType::cancel;
  } else {
    throw std::runtime_error(fmt::format("Invalid type {}", type));
  }

  this->id = std::any_cast<int>(map["id"]);
  this->window_id = xg::Guid(std::any_cast<std::string>(map["windowId"]));

  this->pos = glm::vec2(
    std::any_cast<double>(map["x"]),
    std::any_cast<double>(map["y"])
  );
}
