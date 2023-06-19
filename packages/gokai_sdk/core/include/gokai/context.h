#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <stdint.h>
#include <string>
#include <uv.h>

namespace Gokai {
  class ContextMode {
    public:
      ContextMode();
      ContextMode(uint8_t id, std::string name);

      bool operator!=(ContextMode b);
      bool operator==(ContextMode b);

      static ContextMode fromValue(Value<std::any> value);

      static const ContextMode invalid;
      static const ContextMode compositor;
      static const ContextMode client;
      static const ContextMode values[3];
    private:
      uint8_t id;
      std::string name;
  };

  class Service;
  class Context : public Object {
    public:
      Context(ObjectArguments arguments);
      ~Context();

      ContextMode getMode();
      template<class T> T* getSystemService();
      virtual Service* getSystemService(std::string serviceName);
      virtual std::string getPackageName();
      virtual std::string getPackageDataDir();
      virtual std::string getPackageConfigDir();
    private:
      uv_loop_t* loop;
      ContextMode mode;
  };
}
#endif