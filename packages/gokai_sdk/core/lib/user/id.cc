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
  } else if (value.type() == typeid(int)) {
    this->data = {
      .uid = (uint32_t)std::any_cast<int>(value)
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
      throw std::runtime_error(fmt::format("Invalid type {}", (uint8_t)this->type));
  }
}

bool ID::operator>(const ID& b) const {
  if (b.type != this->type) return false;

  switch (this->type) {
    case guid: return this->data.guid.str() > b.data.guid.str();
    case uid: return this->data.uid > b.data.uid;
    default: return false;
  }
}

bool ID::operator<(const ID& b) const {
  if (b.type != this->type) return false;

  switch (this->type) {
    case guid: return this->data.guid.str() < b.data.guid.str();
    case uid: return this->data.uid < b.data.uid;
    default: return false;
  }
}

bool ID::operator==(const ID& b) const {
  if (b.type != this->type) return false;

  switch (this->type) {
    case guid: return this->data.guid == b.data.guid;
    case uid: return this->data.uid == b.data.uid;
    default: return false;
  }
}
