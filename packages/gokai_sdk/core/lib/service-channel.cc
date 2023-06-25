#include <fmt/core.h>
#include <gokai/service-channel.h>

#define TAG "Gokai::ServiceChannel"

using namespace Gokai;

ServiceChannel::ServiceChannel(ObjectArguments arguments) :
  Loggable(fmt::format("{}#{}", TAG, std::any_cast<std::string>(arguments.get("name"))).c_str(), arguments),
  context{std::any_cast<std::shared_ptr<Context>>(arguments.get("context"))},
  name{std::any_cast<std::string>(arguments.get("name"))} {}

std::string ServiceChannel::getName() {
  return this->name;
}

std::vector<uint8_t> ServiceChannel::receive(xg::Guid engine_id, std::vector<uint8_t> message) {
  for (auto func : this->onReceive) {
    auto result = func(engine_id, message);
    if (result.size() > 0) {
      return result;
    }
  }
  return std::vector<uint8_t>();
}