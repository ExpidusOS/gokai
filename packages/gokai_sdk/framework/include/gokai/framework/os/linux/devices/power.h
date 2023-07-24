#pragma once

#ifdef __cplusplus
#include <gokai/devices/power.h>
#include <upower.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Devices {
          class Power : public Gokai::Devices::Power {
            public:
              Power(Gokai::ObjectArguments arguments);
              ~Power();

              UpDevice* getValue();
              int getCycleCount() override;
              double getLevel() override;
              bool isIntegrated() override;
            private:
              UpDevice* value;
          };
        }
      }
    }
  }
}
#endif
