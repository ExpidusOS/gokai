#include <gokai/context.h>
#include <gokai/services/display-manager.h>
#include <gokai/services/package-manager.h>
#include <gokai/services/window-manager.h>
#include <gokai/values/int.h>
#include <gokai/values/string.h>
#include <assert.h>
#include <stdexcept>
#include <type_traits>

using namespace Gokai;

ContextMode::ContextMode() : ContextMode(-1, "") {}

ContextMode::ContextMode(uint8_t id, std::string name) {
  this->id = id;
  this->name = name;
}

bool ContextMode::operator==(ContextMode b) {
  return this->id == b.id && this->name.compare(b.name) == 0;
}

bool ContextMode::operator!=(ContextMode b) {
  return this->id != b.id || this->name.compare(b.name) != 0;
}

ContextMode ContextMode::fromValue(Value* value) {
  if (dynamic_cast<Values::Integer*>(value)) {
    auto num = dynamic_cast<Values::Integer*>(value);

    for (auto value : ContextMode::values) {
      if (value != ContextMode::invalid) {
        if (value.id == num->getValue()) return value;
      }
    }

    throw std::invalid_argument("Invalid value provided");
  } else if (dynamic_cast<Values::String*>(value)) {
    auto str = dynamic_cast<Values::String*>(value);

    for (auto value : ContextMode::values) {
      if (value != ContextMode::invalid) {
        if (value.name.compare(str->getValue()) == 0) return value;
      }
    }

    throw std::invalid_argument("Invalid value provided");
  }

  throw std::runtime_error("Value is not an integer or string");
}

const ContextMode ContextMode::invalid = ContextMode();
const ContextMode ContextMode::client = ContextMode(0, "client");
const ContextMode ContextMode::compositor = ContextMode(1, "compositor");
const ContextMode ContextMode::values[3] = {
  ContextMode::invalid,
  ContextMode::client,
  ContextMode::compositor,
};

Context::Context(ObjectArguments arguments) : Object(arguments) {
  this->mode = arguments.has("mode") ? ContextMode::fromValue(arguments.getPointed("mode"))
    : ContextMode::client;

  this->loop = reinterpret_cast<uv_loop_t*>(malloc(sizeof (uv_loop_t)));
  assert(this->loop != nullptr);
  uv_loop_init(this->loop);
}

Context::~Context() {
  uv_loop_close(this->loop);
  free(this->loop);
}

ContextMode Context::getMode() {
  return this->mode;
}

template<class T> T* Context::getSystemService() {
  if (std::is_same<T, Services::DisplayManager>::value) {
    return getSystemService(Services::DisplayManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::PackageManager>::value) {
    return getSystemService(Services::PackageManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::WindowManager>::value) {
    return getSystemService(Services::WindowManager::SERVICE_NAME);
  }
  return nullptr;
}

Service* Context::getSystemService(std::string serviceName) {
  return nullptr;
}

std::string Context::getPackageName() {
  return nullptr;
}

std::string Context::getPackageConfigDir() {
  return nullptr;
}

std::string Context::getPackageDataDir() {
  return nullptr;
}