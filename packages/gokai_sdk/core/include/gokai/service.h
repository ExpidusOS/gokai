#pragma once

#ifdef __cplusplus
namespace Gokai {
  class Context;
  class Service {
    public:
      Service(Gokai::Context* context);
    protected:
      Gokai::Context* context;
  };
}
#endif