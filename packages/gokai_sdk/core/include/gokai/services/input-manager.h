#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/flutter/codecs/json.h>
#include <gokai/input/base.h>
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

        std::list<std::function<void()>> changed;

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif
