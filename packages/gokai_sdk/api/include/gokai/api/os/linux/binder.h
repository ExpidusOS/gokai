#pragma once

#ifdef __cplusplus
#include <gokai/api/binder.h>
#include <dlfcn.h>

namespace Gokai {
  namespace API {
    namespace os {
      namespace Linux {
        class Binder : public Gokai::API::Binder {
          public:
            Binder(Gokai::ObjectArguments arguments);
            ~Binder();

            Gokai::ObjectFactory* getObjectFactory() override;
            std::string getPath() override;
          private:
            std::string path;
            void* handle;
        };
      }
    }
  }
}
#endif