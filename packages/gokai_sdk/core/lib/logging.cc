#include <gokai/logging.h>
#include <stdexcept>

using namespace Gokai;

Logger::Logger(ObjectArguments arguments) : Object(arguments) {
  this->logger = nullptr;
}

std::shared_ptr<spdlog::logger> Logger::get(const char* tag) {
  throw std::runtime_error("Logger::get(TAG) has not been implemented.");
}

Loggable::Loggable(const char* tag, ObjectArguments arguments) : Object(arguments),
  obj{std::any_cast<Logger*>(arguments.get("logger"))},
  logger{std::any_cast<Logger*>(arguments.get("logger"))->get(tag)} {}

Logger* Loggable::getLogger() {
  return this->obj;
}