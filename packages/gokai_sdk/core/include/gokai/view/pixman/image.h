#pragma once

#ifdef __cplusplus
#include <gokai/view/displayable.h>
#include <gokai/logging.h>
#include <pixman.h>

namespace Gokai {
  namespace View {
    namespace Pixman {
      class Image : public Gokai::View::Displayable, public Gokai::Loggable {
        public:
          Image(Gokai::ObjectArguments arguments);
          ~Image();

          pixman_image_t* getValue();
          Gokai::View::URect getRect() override;
        private:
          pixman_image_t* value;
      };
    }
  }
}
#endif