#pragma once

#ifdef __cplusplus
#include <string>

namespace Gokai {
  class Service;
  class Context {
    public:
      template<class T> T* getSystemService();
      virtual Service* getSystemService(std::string serviceName);
      virtual std::string getPackageName();
      virtual std::string getPackageDir();
  };
}
#endif