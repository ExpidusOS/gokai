#include <fmt/core.h>
#include <gokai/framework/os/linux/input/wayland/server/base.h>

using namespace Gokai::Framework::os::Linux::Input::Wayland::Server;

Base::Base(Gokai::ObjectArguments arguments) : Gokai::Input::Base(arguments) {
  this->value = std::any_cast<struct wlr_input_device*>(arguments.get("value"));

  this->destroy_listener.notify = Base::破壊する;
  wl_signal_add(&this->value->events.destroy, &this->destroy_listener);
}

Base::~Base() {
  wl_list_remove(&this->destroy_listener.link);

  for (auto func : this->destroy) func();
}

struct wlr_input_device* Base::getValue() {
  return this->value;
}

std::string Base::getName() {
  return fmt::format("{} ({}:{})", this->value->name, reinterpret_cast<void*>(this->value), this->value->vendor, this->value->product);
}

void Base::破壊する(struct wl_listener* listener, void* data) {
  Base* self = wl_container_of(listener, self, destroy_listener);
  delete self;
}