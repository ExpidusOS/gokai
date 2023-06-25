#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
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

        std::shared_ptr<Gokai::Flutter::Engine> create(Gokai::Graphics::Renderer* renderer);
        void destroy(xg::Guid id);

        static const std::string SERVICE_NAME;
      private:
        std::map<xg::Guid, Gokai::Flutter::Engine*> engines;
    };
  }
}
#endif