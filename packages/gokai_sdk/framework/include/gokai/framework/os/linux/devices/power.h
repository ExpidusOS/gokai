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
              bool isCharging() override;
            private:
              UpDevice* value;
              gulong energy_id;
              gulong state_id;

              static void energy_callback(GObject* obj, GParamSpec* pspec, gpointer data);
              static void state_callback(GObject* obj, GParamSpec* pspec, gpointer data);
          };
        }
      }
    }
  }
}
#endif
