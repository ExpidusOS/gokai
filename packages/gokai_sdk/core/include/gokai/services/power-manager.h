#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/devices/power.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>

namespace Gokai {
  namespace Services {
    class PowerManager : public Gokai::Service, public Gokai::Loggable {
      public:
        PowerManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        std::list<std::function<void()>> changed;

        virtual std::list<xg::Guid> getIds();
        virtual std::shared_ptr<Gokai::Devices::Power> get(xg::Guid id);

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif
