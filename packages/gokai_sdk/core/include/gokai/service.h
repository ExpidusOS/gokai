#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <gokai/service-channel.h>
#include <memory>

namespace Gokai {
  class Context;
  class Service : public Object {
    public:
      Service(ObjectArguments arguments);

      virtual std::shared_ptr<Gokai::ServiceChannel> getServiceChannel();
    protected:
      std::shared_ptr<Gokai::Context> context;
  };
}
#endif