#pragma once

#ifdef __cplusplus
#include <gokai/services/window-manager.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class WindowManager : public Gokai::Services::WindowManager {
                public:
                  WindowManager(Gokai::ObjectArguments arguments);
              };
            }
          }
        }
      }
    }
  }
}
#endif