#pragma once

#ifdef __cplusplus
#include <gokai/view/rect.h>
#include <gokai/object.h>
#include <list>

namespace Gokai {
  namespace View {
    struct DisplayMode {
      URect resolution;
      double refresh;
      std::list<HDRType> hdr_types;
    };

    class Display : public Gokai::Object {
      public:
        Display(Gokai::ObjectArguments arguments);

        virtual bool isHDR();
        virtual glm::uvec2 getPhysicalSize();
        virtual std::list<DisplayMode> getModes();
        virtual void setMode(DisplayMode mode);
    };
  }
}
#endif