#pragma once

#ifdef __cplusplus
#include <gokai/services/compositor.h>

extern "C" {
#define static
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/gles2.h>
#include <wlr/render/pixman.h>
#include <wlr/render/vulkan.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_drm_lease_v1.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_viewporter.h>
#include <wlr/util/log.h>
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
              class Compositor : public Gokai::Services::Compositor {
                public:
                  Compositor(Gokai::ObjectArguments arguments);
                  ~Compositor();

                  void start();
                  struct wl_display* getDisplay();
                  struct wlr_backend* getBackend();
                  struct wlr_renderer* getRenderer();
                  struct wlr_allocator* getAllocator();
                  struct wlr_drm_lease_v1_manager* getDrmLeaseManager();
                private:
                  static void poll_event_handle(uv_poll_t* event_poll, int status, int events);

                  struct wl_display* display;
                  struct wlr_backend* backend;
                  struct wlr_renderer* renderer;
                  struct wlr_allocator* allocator;
                  struct wlr_drm_lease_v1_manager* drm_lease_manager;
                  uv_poll_t event_poll;
                  const char* socket;
              };
            }
          }
        }
      }
    }
  }
}
#endif
