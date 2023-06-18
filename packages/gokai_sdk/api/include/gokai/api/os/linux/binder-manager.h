#pragma once

#ifdef __cplusplus
#include <gokai/api/binder-manager.h>

namespace Gokai {
  namespace API {
    namespace os {
      namespace Linux {
        class BinderManager : public Gokai::API::BinderManager {
          public:
            BinderManager(Gokai::ObjectArguments arguments);

            Gokai::API::Binder* load(std::string name) override;
            std::map<std::string, Gokai::API::Binder*> getAll() override;
        };
      }
    }
  }
}
#endif