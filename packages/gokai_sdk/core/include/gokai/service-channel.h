#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <string>

namespace Gokai {
  class ServiceChannel : public Object {
    public:
      ServiceChannel(ObjectArguments arguments);
  };
}
#endif