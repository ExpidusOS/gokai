#pragma once

#ifdef __cplusplus
#include <any>

namespace Gokai {
  template<typename T> class Value {
    public:
      Value(T value);

      bool isSet();
      bool isEmpty();
      void* toPointer();
      T getValue();
    private:
      T value;
  };

  template class Value<std::any>;
}
#endif