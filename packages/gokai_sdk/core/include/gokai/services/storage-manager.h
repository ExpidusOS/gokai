#pragma once

#ifdef __cplusplus
#include <gokai/flutter/codecs/json.h>
#include <gokai/fs/entry.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>

namespace Gokai {
  namespace Services {
    class StorageManager : public Gokai::Service, public Gokai::Loggable {
      public:
        StorageManager(Gokai::ObjectArguments arguments);

        std::shared_ptr<Gokai::ServiceChannel> getServiceChannel() override;

        virtual Gokai::FS::Entry open(std::filesystem::path path);

        static const std::string SERVICE_NAME;
      private:
        Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
        std::shared_ptr<Gokai::ServiceChannel> service_channel;
    };
  }
}
#endif
