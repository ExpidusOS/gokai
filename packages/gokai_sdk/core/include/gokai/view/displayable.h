#pragma once

#ifdef __cplusplus
#include <gokai/view/rect.h>
#include <gokai/object.h>
#include <functional>
#include <list>

namespace Gokai {
  namespace View {
    class Displayable : Gokai::Object {
      public:
        Displayable(Gokai::ObjectArguments arguments);

        std::list<std::function<bool(glm::uvec2)>> onResize;
        std::list<std::function<bool(glm::uvec2)>> onFrame;

        virtual bool resize(glm::uvec2 size);
        virtual bool frame(glm::uvec2 size);
        virtual URect getRect();
    };
  }
}
#endif