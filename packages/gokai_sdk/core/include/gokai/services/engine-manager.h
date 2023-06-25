#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/flutter/codecs/json.h>
#include <gokai/flutter/engine.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>

namespace Gokai {
  namespace Services {
    class EngineManager : public Gokai::Service, public Gokai::Loggable {
      public:
        EngineManager(Gokai::ObjectArguments arguments);
        ~EngineManager();

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        std::shared_ptr<Gokai::Flutter::Engine> create(Gokai::Graphics::Renderer* renderer);
        void destroy(xg::Guid id);

        std::list<xg::Guid> getIds();
        std::shared_ptr<Gokai::Flutter::Engine> get(xg::Guid id);

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        Gokai::ServiceChannel* service_channel;
        std::map<xg::Guid, Gokai::Flutter::Engine*> engines;
    };
  }
}
#endif