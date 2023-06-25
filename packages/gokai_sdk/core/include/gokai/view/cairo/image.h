#pragma once

#ifdef __cplusplus
#include <gokai/view/displayable.h>
#include <gokai/logging.h>
#include <cairo.h>

namespace Gokai {
  namespace View {
    namespace Cairo {
      class Image : public Gokai::View::Displayable, public Gokai::Loggable {
        public:
          Image(Gokai::ObjectArguments arguments);
          ~Image();

          cairo_surface_t* getValue();
          Gokai::View::URect getRect() override;
        private:
          cairo_surface_t* value;
      };
    }
  }
}
#endif