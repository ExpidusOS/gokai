#pragma once

#ifdef __cplusplus
#include <gokai/flutter/codecs/json.h>
#include <gokai/flutter/text-input-model.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>

namespace Gokai {
  namespace Services {
    class CompositorInputMethod : public Gokai::Service, public Gokai::Loggable {
      public:
        CompositorInputMethod(Gokai::ObjectArguments arguments);
        Gokai::Flutter::TextInputModel model;

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        bool sendStateUpdate(xg::Guid engine_id);
        bool performActionMethod(xg::Guid engine_id);

        virtual void showInput();
        virtual void hideInput();
        bool isActive();
        std::string getInputType();

        static const std::string SERVICE_NAME;
      protected:
        bool is_active;
      private:
        std::string input_type;
        std::string input_action;
        int client_id;
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif