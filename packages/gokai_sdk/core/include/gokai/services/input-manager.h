#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/flutter/codecs/json.h>
#include <gokai/input/base.h>
#include <gokai/input/pointer.h>
#include <gokai/input/touch.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <glm/vec2.hpp>
#include <string>

namespace Gokai {
  namespace Services {
    class InputManager : public Gokai::Service, public Gokai::Loggable {
      public:
        InputManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        virtual std::list<xg::Guid> getIds();
        virtual std::shared_ptr<Gokai::Input::Base> get(xg::Guid id);
        virtual glm::uvec2 getActivePoint();

        virtual bool sendPointer(Gokai::Input::Pointer::Event event);
        virtual bool sendTouch(Gokai::Input::Touch::Event event);

        std::list<std::function<void()>> changed;
        std::list<std::function<void(bool)>> onLid;
        std::list<std::function<void(bool)>> onTablet;

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif
