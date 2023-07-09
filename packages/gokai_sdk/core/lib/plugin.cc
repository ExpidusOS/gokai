#include <gokai/plugin.h>
#include <assert.h>

using namespace Gokai;

Plugin::Plugin(Gokai::ObjectArguments arguments) : Loggable(std::any_cast<const char*>(arguments.get("tag")), arguments) {
  auto context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;
}
