#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          namespace Wayland {
            namespace Server {
              class Compositor : public Gokai::Services::Compositor {
                public:
                  Compositor(Gokai::ObjectArguments arguments);
              };
            }
          }
        }
      }
    }
  }
}
#endif