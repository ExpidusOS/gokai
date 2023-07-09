#pragma once

#ifdef __cplusplus
#include <gokai/flutter/codecs/json.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/plugin-module.h>
#include <gokai/service.h>
#include <list>
#include <string>

namespace Gokai {
  namespace Services {
    class PluginManager : public Gokai::Service, public Gokai::Loggable {
      public:
        PluginManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        std::list<std::string> paths;
        void reload();

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
        std::map<std::string, Gokai::PluginModule*> map;
    };
  }
}
#endif
