#include <gokai/service.h>
#include <assert.h>
#include <stdexcept>

using namespace Gokai;

Service::Service(ObjectArguments arguments) : Object(arguments) {
  auto context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;
}

std::shared_ptr<ServiceChannel> Service::getServiceChannel() {
  return nullptr;
}