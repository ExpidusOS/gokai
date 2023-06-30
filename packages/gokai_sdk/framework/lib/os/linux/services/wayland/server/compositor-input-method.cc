#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor-input-method.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

CompositorInputMethod::CompositorInputMethod(Gokai::ObjectArguments arguments) : Gokai::Services::CompositorInputMethod(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));
  this->manager = wlr_input_method_manager_v2_create(compositor->getDisplay());
}