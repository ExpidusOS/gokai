#include <fmt/format.h>
#include <gokai/casting.h>

uint64_t Gokai::Casting::castInt(std::any value) {
  if (value.type() == typeid(int)) {
    return std::any_cast<int>(value);
  } else if (value.type() == typeid(int64_t)) {
    return std::any_cast<int64_t>(value);
  } else if (value.type() == typeid(uint64_t)) {
    return std::any_cast<uint64_t>(value);
  } else if (value.type() == typeid(unsigned int)) {
    return std::any_cast<unsigned int>(value);
  } else {
    throw std::runtime_error(fmt::format("Cannot cast {} to an integer type", value.type().name()));
  }
}
