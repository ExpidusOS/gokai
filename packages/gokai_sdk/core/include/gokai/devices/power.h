#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/object.h>

namespace Gokai {
  namespace Devices {
    class Power : public Gokai::Object {
      public:
        Power(Gokai::ObjectArguments arguments);

        xg::Guid getId();
        virtual int getCycleCount();
        virtual double getLevel();
        virtual bool isIntegrated();
      private:
        xg::Guid id;
    };
  }
}
#endif
