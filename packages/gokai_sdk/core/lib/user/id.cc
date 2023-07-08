#include <fmt/core.h>
#include <gokai/user/id.h>
#include <stdexcept>

using namespace Gokai::User;

ID::ID(xg::Guid guid) : type{Gokai::User::guid}, data{.guid = guid} {}
ID::ID(uint32_t uid) : type{Gokai::User::uid}, data{.uid = uid} {}

ID::ID(std::any value) : ID(-1) {
  if (value.type() == typeid(uint32_t)) {
    this->data = {
      .uid = std::any_cast<uint32_t>(value)
    };
  } else if (value.type() == typeid(uint64_t)) {
    this->data = {
      .uid = (uint32_t)std::any_cast<uint64_t>(value)
    };
  } else if (value.type() == typeid(std::string)) {
    this->type = Gokai::User::guid;
    this->data = {
      .guid = xg::Guid(std::any_cast<std::string>(value))
    };
  } else {
    throw std::runtime_error(fmt::format("Invalid type {}", value.type().name()));
  }
}

std::any ID::toAny() {
  switch (this->type) {
    case guid: return std::any(this->data.guid.str());
    case uid: return std::any(this->data.uid);
    default:
      throw std::runtime_error(fmt::format("Invalid type {}", this->type));
  }
}
