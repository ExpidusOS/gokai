#pragma once

#ifdef __cplusplus
#include <gokai/services/display-manager.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class DisplayManager : public Gokai::Services::DisplayManager {
                public:
                  DisplayManager(Gokai::ObjectArguments arguments);
              };
            }
          }
        }
      }
    }
  }
}
#endif