#include <gokai/framework/os/linux/services/input-manager.h>
#include <filesystem>
#include <string>

using namespace Gokai::Framework::os::Linux::Services;

InputManager::InputManager(Gokai::ObjectArguments arguments) : Gokai::Services::InputManager(arguments) {
  uv_fs_event_init(this->context->getLoop(), &this->input_watch_handle);
  uv_fs_event_start(&this->input_watch_handle, [](auto handle, auto path, auto ev, auto status) {
    InputManager* self = reinterpret_cast<InputManager*>((char*)(handle) - offsetof(InputManager, input_watch_handle));
    self->rescan();
  }, "/dev/input/by-path", UV_FS_EVENT_WATCH_ENTRY);

  this->rescan();
}

InputManager::~InputManager() {
  uv_fs_event_stop(&this->input_watch_handle);
}

std::list<xg::Guid> InputManager::getIds() {
  std::list<xg::Guid> list;
  for (const auto& value : this->inputs) {
    list.push_back(value.second->getId());
  }
  return list;
}

std::shared_ptr<Gokai::Input::Base> InputManager::get(xg::Guid id) {
  for (const auto& value : this->inputs) {
    if (value.second->getId() == id) {
      return std::shared_ptr<Gokai::Input::Base>(value.second);
    }
  }
  return nullptr;
}

void InputManager::rescan() {
  for (const auto& value : this->inputs) {
    if (!std::filesystem::exists(value.second->getPath())) {
      this->inputs.erase(value.first);
      delete value.second;
    }
  }

  for (const auto& entry : std::filesystem::directory_iterator("/dev/input/by-path")) {
    if (entry.path().string().size() >= 14 && 0 == entry.path().string().compare(entry.path().string().size() - 14, 14, "event-joystick")) {
      auto find = this->inputs.find(entry.path());
      if (find != this->inputs.end()) continue;

      try {
        auto device = new Gokai::Framework::os::Linux::Input::Base(Gokai::ObjectArguments({
          { "id", xg::newGuid() },
          { "context", this->context },
          { "logger", this->getLogger() },
          { "path", entry.path().string() },
        }));

        this->inputs[entry.path()] = device;
        for (auto func : this->changed) func();
      } catch (const std::exception& ex) {
        this->logger->error("Failed to create input device for {}: {}", entry.path().string(), ex.what());
      }
    }
  }
}
