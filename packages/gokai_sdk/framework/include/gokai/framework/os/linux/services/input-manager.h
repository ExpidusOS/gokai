#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/base.h>
#include <gokai/services/input-manager.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class InputManager : public Gokai::Services::InputManager {
            public:
              InputManager(Gokai::ObjectArguments arguments);
              ~InputManager();

              std::list<std::string> getNames() override;
              std::shared_ptr<Gokai::Input::Base> get(std::string name) override;
            private:
              uv_fs_event_t input_watch_handle;
              std::map<std::string, Gokai::Framework::os::Linux::Input::Base*> inputs;

              void rescan();
          };
        }
      }
    }
  }
}
#endif
