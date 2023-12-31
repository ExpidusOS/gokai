#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/flutter/codecs/json.h>
#include <gokai/flutter/engine.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <future>

namespace Gokai {
  namespace Services {
    class EngineManager : public Gokai::Service, public Gokai::Loggable {
      public:
        EngineManager(Gokai::ObjectArguments arguments);
        ~EngineManager();

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        std::shared_ptr<Gokai::Flutter::Engine> create(
          Gokai::Graphics::Renderer* renderer,
          Gokai::Flutter::EngineViewType type,
          std::string name
        );
        void destroy(xg::Guid id);

        std::list<xg::Guid> getIds();
        Gokai::Flutter::Engine* get(xg::Guid id);
        std::map<xg::Guid, std::future<std::vector<uint8_t>>> sendAll(std::string channel, std::vector<uint8_t> data);

        std::list<std::function<void()>> changed;

        static const std::string SERVICE_NAME;
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
      private:
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
        std::map<xg::Guid, Gokai::Flutter::Engine*> engines;
    };
  }
}
#endif
