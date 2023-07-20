#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <libevdev/libevdev.h>
#include <list>
#include <functional>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Input {
          class Base : public Gokai::Input::Base {
            public:
              Base(Gokai::ObjectArguments arguments);
              ~Base();

              struct libevdev* getValue();
              std::string getPath();

              std::string getName() override;

              std::list<std::function<void()>> destroy;
            protected:
              virtual void handleEvent(struct input_event event);
            private:
              uv_poll_t handle;
              struct libevdev* value;
              std::string path;
          };
        }
      }
    }
  }
}
#endif