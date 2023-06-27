#include <gokai/framework/os/linux/input/wayland/server/keyboard.h>
#include <gokai/framework/os/linux/input/wayland/server/pointer.h>
#include <gokai/framework/os/linux/services/wayland/server/compositor.h>
#include <gokai/framework/os/linux/services/wayland/server/input-manager.h>

using namespace Gokai::Framework::os::Linux::Services::Wayland::Server;

void InputManager::handle_input_new(struct wl_listener* listener, void* data) {
  InputManager* self = wl_container_of(listener, self, input_new);
  auto value = reinterpret_cast<struct wlr_input_device*>(data);

  switch (value->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
      try {
        auto input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Keyboard(Gokai::ObjectArguments({
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));

        input->destroy.push_back([input, self]() {
          self->inputs.remove(input);
        });

        self->inputs.push_back(input);
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create keyboard: {}", ex.what());
      }
      break;
    case WLR_INPUT_DEVICE_POINTER:
      try {
        auto input = new Gokai::Framework::os::Linux::Input::Wayland::Server::Pointer(Gokai::ObjectArguments({
          { "context", self->context },
          { "logger", self->getLogger() },
          { "value", value },
        }));

        input->destroy.push_back([input, self]() {
          self->inputs.remove(input);
        });

        self->inputs.push_back(input);
      } catch (const std::exception& ex) {
        self->logger->error("Failed to create pointer: {}", ex.what());
      }
      break;
    default:
      self->logger->warn("Unknown device type {}", value->type);
      break;
  }
}

InputManager::InputManager(Gokai::ObjectArguments arguments) : Gokai::Services::InputManager(arguments) {
  auto compositor = reinterpret_cast<Compositor*>(this->context->getSystemService(Gokai::Services::Compositor::SERVICE_NAME));

  wl_signal_add(&compositor->getBackend()->events.new_input, &this->input_new);
  this->input_new.notify = InputManager::handle_input_new;
}

std::list<std::string> InputManager::getNames() {
  std::list<std::string> list;
  for (const auto& value : this->inputs) {
    list.push_back(value->getName());
  }
  return list;
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(std::string name) {
  for (const auto& value : this->inputs) {
    if (value->getName().compare(name) == 0) {
      return std::shared_ptr<Gokai::Input::Base>(value);
    }
  }
  return nullptr;
}