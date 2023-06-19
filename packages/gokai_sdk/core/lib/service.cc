#include <gokai/service.h>
#include <assert.h>
#include <stdexcept>

using namespace Gokai;

Service::Service(ObjectArguments arguments) : Object(arguments) {
  Context* context = std::any_cast<Context*>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;
}

ServiceChannel* Service::getServiceChannel() {
  return nullptr;
}