#include <gokai/service-channel.h>

using namespace Gokai;

ServiceChannel::ServiceChannel(ObjectArguments arguments) : Object(arguments) {}

std::string ServiceChannel::getName() {
  return std::string();
}