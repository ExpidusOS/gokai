#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/input/base.h>
#include <gokai/logging.h>
#include <glm/glm.hpp>

namespace Gokai {
  namespace Input {
    class Touch : public Base, public Gokai::Loggable {
      public:
        Touch(Gokai::ObjectArguments arguments);

        enum class EventType {
          up,
          down,
          motion,
          cancel,
        };

        struct Event {
          EventType type;
          int id;
          xg::Guid window_id;
          glm::vec2 pos;

          Event(std::any any);
          Event(std::map<std::string, std::any> map);
        };
    };
  }
}
#endif
