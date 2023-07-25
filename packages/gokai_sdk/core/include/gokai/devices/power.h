#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/context.h>
#include <gokai/object.h>
#include <functional>
#include <list>
#include <memory>

namespace Gokai {
  namespace Devices {
    class Power : public Gokai::Object {
      public:
        Power(Gokai::ObjectArguments arguments);

        std::list<std::function<void()>> onChange;

        xg::Guid getId();
        virtual std::string getName();
        virtual int getCycleCount();
        virtual double getLevel();
        virtual bool isIntegrated();
        virtual bool isCharging();
      protected:
        std::shared_ptr<Gokai::Context> context;
      private:
        xg::Guid id;
    };
  }
}
#endif
