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
        std::list<std::function<void()>> onMapped;
        std::list<std::function<void()>> onEnter;
        std::list<std::function<void()>> onLeave;
        std::list<std::function<void()>> onActive;

        virtual bool isToplevel();
        virtual std::string getRole();
        virtual std::list<xg::Guid> getChildrenAbove();
        virtual std::list<xg::Guid> getChildrenBelow();
        virtual std::string getDisplayName();
        virtual bool hasDecorations();
        virtual bool hasTexture();
        virtual int64_t getTextureId();
        virtual std::shared_ptr<Gokai::Graphics::Texture> getTexture();
        virtual bool isMapped();
        virtual void setMapped(bool value);
        virtual bool isActive();
        virtual void setActive(bool value);

        xg::Guid getId();
        virtual URect getRect();
        virtual void setRect(URect rect);

        virtual std::string getTitle();
      protected:
        std::shared_ptr<Gokai::Context> context;
      private:
        xg::Guid id;
        URect rect;
        bool is_mapped;
        bool is_active;
    };
  }
}
#endif
