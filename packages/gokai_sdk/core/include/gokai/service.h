#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <gokai/service-channel.h>

namespace Gokai {
  class Context;
  class Service : public Object {
    public:
      Service(ObjectArguments arguments);

      virtual ServiceChannel* getServiceChannel();
    protected:
      Gokai::Context* context;
  };
}
#endif