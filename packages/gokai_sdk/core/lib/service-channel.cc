#include <fmt/core.h>
#include <gokai/service-channel.h>

#define TAG "Gokai::ServiceChannel"

using namespace Gokai;

static std::string get_name(ObjectArguments arguments) {
  if (arguments.has("names")) {
    return std::any_cast<std::list<std::string>>(arguments.get("names")).front();
  }
  return std::any_cast<std::string>(arguments.get("name"));
}

ServiceChannel::ServiceChannel(ObjectArguments arguments) :
  Loggable(fmt::format("{}#{}", TAG, get_name(arguments)).c_str(), arguments),
  context{std::any_cast<std::shared_ptr<Context>>(arguments.get("context"))} {

  auto name = get_name(arguments);

  if (arguments.has("names")) {
    this->names = std::any_cast<std::list<std::string>>(arguments.get("names"));
  }

  if (!this->accepts(name)) {
    this->names.push_back(name);
  }

  this->logger->debug("Service channel created");
}

std::list<std::string> ServiceChannel::getNames() {
  return this->names;
}

bool ServiceChannel::accepts(std::string channel) {
  for (auto name : this->names) {
    if (name.compare(channel) == 0) return true;
  }
  return false;
}

std::vector<uint8_t> ServiceChannel::receive(xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
  for (auto func : this->onReceive) {
    auto result = func(engine_id, channel, message);
    if (result.size() > 0) {
      return result;
    }
  }
  return std::vector<uint8_t>();
}
