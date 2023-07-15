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

        std::list<std::function<void()>> onCommit;
        std::list<std::function<void()>> onEnter;
        std::list<std::function<void()>> onLeave;

        virtual std::string getDisplayName();
        virtual bool hasTexture();
        virtual int64_t getTextureId();
        virtual std::shared_ptr<Gokai::Graphics::Texture> getTexture();

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