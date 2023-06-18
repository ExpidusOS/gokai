#pragma once

#ifdef __cplusplus
#include <gokai/object-factory.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        class ObjectFactory : public Gokai::ObjectFactory {
          public:
            ObjectFactory(Gokai::ObjectArguments arguments);

            Gokai::Object create(std::string name, Gokai::ObjectArguments arguments) override;
            Gokai::Object* createPointer(std::string name, Gokai::ObjectArguments arguments) override;
        };
      }
    }
  }
}

extern "C" Gokai::ObjectFactory* gokai_framework_object_factory_get_default();
#endif