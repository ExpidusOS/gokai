#pragma once

#ifdef __cplusplus
#include <gokai/context.h>

namespace Gokai {
  class Application : public Object {
    public:
      Application(ObjectArguments arguments);

      Context* getContext();
    private:
      Context* context;
  };
}
#endif