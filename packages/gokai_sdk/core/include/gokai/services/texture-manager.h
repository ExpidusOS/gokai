#pragma once

#ifdef __cplusplus
#include <gokai/graphics/texture.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class TextureManager : public Gokai::Service, public Gokai::Loggable {
      public:
        TextureManager(Gokai::ObjectArguments arguments);

        int64_t allocate(std::shared_ptr<Gokai::Graphics::Texture> texture);
        std::shared_ptr<Gokai::Graphics::Texture> get(int64_t id);
        void unregister(int64_t id);

        static const std::string SERVICE_NAME;
      private:
        int64_t next_id;
        std::map<int64_t, std::shared_ptr<Gokai::Graphics::Texture>> map;
    };
  }
}
#endif