#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/devices/power.h>
#include <gokai/services/power-manager.h>
#include <upower.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class PowerManager : public Gokai::Services::PowerManager {
            public:
              PowerManager(Gokai::ObjectArguments arguments);
              ~PowerManager();

              std::list<xg::Guid> getIds() override;
              std::shared_ptr<Gokai::Devices::Power> get(xg::Guid id) override;
            private:
              UpClient* client;
              gulong device_added_id;
              gulong device_removed_id;

              std::list<std::shared_ptr<Gokai::Framework::os::Linux::Devices::Power>> devices;

              static void device_added(UpClient* client, UpDevice* device, gpointer data);
              static void device_removed(UpClient* client, const char* device_obj_path, gpointer data);
          };
        }
      }
    }
  }
}
#endif
