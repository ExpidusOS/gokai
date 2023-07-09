#pragma once

#ifdef __cplusplus
#include <gokai/os/dynamic-library.h>

namespace Gokai {
  namespace os {
    namespace Linux {
      class DynamicLibrary : public Gokai::os::DynamicLibrary {
        public:
          DynamicLibrary(Gokai::ObjectArguments arguments);
          ~DynamicLibrary();

          void* lookup(std::string sym) override;
          bool providesSymbol(std::string sym) override;
        private:
          void* handle;
      };
    }
  }
}
#endif
