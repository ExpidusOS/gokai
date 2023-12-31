#pragma once

#ifdef __cplusplus
#include <gokai/framework/os/linux/input/wayland/server/base.h>
#include <gokai/framework/os/linux/services/input-manager.h>
#include <gokai/input/base.h>
#include <gokai/services/input-manager.h>

extern "C" {
#include <wayland-server-core.h>
#define static
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
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
              class InputManager : public Gokai::Framework::os::Linux::Services::InputManager {
                public:
                  InputManager(Gokai::ObjectArguments arguments);

                  struct wlr_xcursor_manager* getXcursorManager();
                  struct wlr_seat* getSeat();

                  std::list<xg::Guid> getIds() override;
                  std::shared_ptr<Gokai::Input::Base> get(xg::Guid id) override;
                  glm::uvec2 getActivePoint() override;
                  void setActivePoint(glm::uvec2 point);

                  bool sendPointer(Gokai::Input::Pointer::Event event) override;
                  bool sendTouch(Gokai::Input::Touch::Event event) override;
                  bool sendText(xg::Guid window_id, std::string text) override;
                  bool sendKeyboard(xg::Guid window_id, int keycode) override;
                private:
                  static void handle_input_new(struct wl_listener* listener, void* data);
                  static void handle_cursor_request(struct wl_listener* listener, void* data);

                  void rescan();

                  struct wlr_xcursor_manager* xcursor_manager;
                  struct wlr_seat* seat;
                  glm::uvec2 active_point;

                  struct wl_listener input_new;
                  struct wl_listener cursor_request;
                  std::map<xg::Guid, Gokai::Framework::os::Linux::Input::Wayland::Server::Base*> inputs;
              };
            }
          }
        }
      }
    }
  }
}
#endif
