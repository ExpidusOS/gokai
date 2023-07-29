#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/input/base.h>
#include <gokai/logging.h>
#include <glm/glm.hpp>

namespace Gokai {
  namespace Input {
    class Pointer : public Base, public Gokai::Loggable {
      public:
        Pointer(Gokai::ObjectArguments arguments);

        enum class EventType {
          hover,
          button,
          leave,
        };

        struct Event {
          EventType type;
          xg::Guid window_id;
          glm::vec2 pos;
          int button;
          bool is_released;

          Event(std::any any);
          Event(std::map<std::string, std::any> map);
        };
    };
  }
}
#endif
