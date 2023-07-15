#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/graphics/texture.h>
#include <gokai/view/rect.h>
#include <gokai/object.h>

namespace Gokai {
  namespace View {
    class Window : public Gokai::Object {
      public:
        Window(Gokai::ObjectArguments arguments);

        virtual std::string getDisplayName();
        virtual bool hasTexture();
        virtual Gokai::Graphics::Texture* getTexture();

        xg::Guid getId();
        virtual URect getRect();
        virtual void setRect(URect rect);
      private:
        xg::Guid id;
        URect rect;
    };
  }
}
#endif