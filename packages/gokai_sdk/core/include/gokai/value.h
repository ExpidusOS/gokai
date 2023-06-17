#pragma once

#ifdef __cplusplus
namespace Gokai {
  class Value {
    public:
      virtual bool isSet();
      virtual bool isEmpty();
      virtual void* toPointer();
  };
}
#endif