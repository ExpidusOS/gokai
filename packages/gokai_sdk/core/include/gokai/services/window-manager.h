#pragma once

#ifdef __cplusplus
#include <gokai/view/window.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class WindowManager : public Gokai::Service, public Gokai::Loggable {
      public:
        WindowManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        std::list<std::function<void()>> changed;

        virtual std::list<xg::Guid> getIds();
        virtual Gokai::View::Window* get(xg::Guid id);

        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        static const std::string SERVICE_NAME;
      protected:
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif