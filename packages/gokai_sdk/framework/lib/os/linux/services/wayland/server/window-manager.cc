#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/window-manager.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Gokai::Services::WindowManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  this->compositor = wlr_compositor_create(compositor->getDisplay(), compositor->getRenderer());
  this->subcompositor = wlr_subcompositor_create(compositor->getDisplay());

  this->xdg_shell = wlr_xdg_shell_create(compositor->getDisplay(), xdg_wm_base_interface.version);
}

struct wlr_compositor* WindowManager::getCompositor() {
  return this->compositor;
}

struct wlr_subcompositor* WindowManager::getSubcompositor() {
  return this->subcompositor;
}

struct wlr_xdg_shell* WindowManager::getXdgShell() {
  return this->xdg_shell;
}