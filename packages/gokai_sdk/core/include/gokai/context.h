#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <string>
#include <uv.h>

namespace Gokai {
  class Service;
  class Context : public Object {
    public:
      Context(ObjectArguments arguments);
      ~Context();

      template<class T> T* getSystemService();
      virtual Service* getSystemService(std::string serviceName);
      virtual std::string getPackageName();
      virtual std::string getPackageDir();
    private:
      uv_loop_t* loop;
  };
}
#endif