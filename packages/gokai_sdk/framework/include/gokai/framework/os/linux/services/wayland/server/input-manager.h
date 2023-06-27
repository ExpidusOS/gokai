#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/input/base.h>
#include <gokai/services/input-manager.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_input_device.h>
#include <wayland-server.h>
#undef static
}

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class InputManager : public Gokai::Services::InputManager {
                public:
                  InputManager(Gokai::ObjectArguments arguments);

                  std::list<std::string> getNames() override;
                  std::shared_ptr<Gokai::Input::Base> get(std::string name) override;
                private:
                  static void handle_input_new(struct wl_listener* listener, void* data);

                  struct wl_listener input_new;
                  std::list<Gokai::Framework::os::Linux::Input::Wayland::Server::Base*> inputs;
              };
            }
          }
        }
      }
    }
  }
}
#endif