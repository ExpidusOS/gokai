#pragma once

#ifdef __cplusplus
#include <gokai/flutter/codecs/json.h>
#include <gokai/user/account.h>
#include <gokai/user/id.h>
#include <gokai/context.h>
#include <gokai/service.h>
#include <functional>

namespace Gokai {
  namespace Services {
    class AccountManager : public Gokai::Service, public Gokai::Loggable {
      public:
        AccountManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        virtual Gokai::User::ID getCurrentId();
        virtual std::list<Gokai::User::ID> getIds();
        virtual Gokai::User::Account* get(Gokai::User::ID id);
        Gokai::User::Account* get(xg::Guid guid);
        Gokai::User::Account* get(uint32_t id);

        std::list<std::function<void()>> changed;

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif
