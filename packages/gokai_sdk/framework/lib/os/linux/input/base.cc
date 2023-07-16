#include <fmt/core.h>
#include <gokai/framework/os/linux/input/base.h>
#include <gokai/framework/os/linux/services/input-manager.h>
#include <spdlog/spdlog.h>

using namespace Gokai::Framework::os::Linux::Input;

Base::Base(Gokai::ObjectArguments arguments) : Gokai::Input::Base(arguments), path{std::any_cast<std::string>(arguments.get("path"))} {
  int fd = open(this->path.c_str(), O_RDONLY | O_NONBLOCK);
  int rc = libevdev_new_from_fd(fd, &this->value);
  if (rc < 0) {
    throw std::runtime_error("Failed to initialize device");
  }

  uv_poll_init(this->context->getLoop(), &this->handle, fd);
  uv_poll_start(&this->handle, UV_READABLE, [](auto handle, auto status, auto events) {
    Base* self = reinterpret_cast<Base*>((char*)(handle) - offsetof(Base, handle));
    if (libevdev_has_event_pending(self->value)) {
      struct input_event event;
      int rc = libevdev_next_event(self->value, LIBEVDEV_READ_FLAG_NORMAL, &event);
      if (rc == 0) {
        std::map<std::string, std::any> ev;
        switch (event.type) {
          case EV_KEY:
            ev["type"] = "button";
            if (event.code >= BTN_MISC) {
              auto value = libevdev_event_code_get_name(EV_KEY, event.code);
              if (value != nullptr) ev["key"] = value;
            }
            break;
          case EV_ABS:
          case EV_REL:
            ev["type"] = "analog";
            break;
        }
      }
    }
  });
}

Base::~Base() {
  for (auto func : this->destroy) func();

  uv_poll_stop(&this->handle);
  libevdev_free(this->value);
}

struct libevdev* Base::getValue() {
  return this->value;
}

std::string Base::getPath() {
  return this->path;
}

std::string Base::getName() {
  return fmt::format(
    "{} ({:x}-{:x}-{:x})",
    libevdev_get_name(this->value),
    libevdev_get_id_bustype(this->value),
    libevdev_get_id_vendor(this->value),
    libevdev_get_id_product(this->value)
  );
}