#pragma once

#ifdef __cplusplus
#include <gokai/object.h>

namespace Gokai {
  namespace os {
    class DynamicLibrary : public Gokai::Object {
      public:
        DynamicLibrary(Gokai::ObjectArguments arguments);

        virtual void* lookup(std::string sym);
        virtual bool providesSymbol(std::string sym);

        template<typename T> T lookup(std::string sym) {
          return reinterpret_cast<T>(this->lookup(sym));
        }

        static DynamicLibrary* open(std::string path);
    };
  }
}
#endif
