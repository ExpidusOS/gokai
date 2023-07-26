#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/flutter/codecs/json.h>
#include <gokai/application-manifest.h>
#include <gokai/logging.h>
#include <gokai/object.h>
#include <future>
#include <stdint.h>
#include <string>
#include <uv.h>

namespace Gokai {
  struct ContextMode {
    uint8_t id;
    std::string name;

    ContextMode();
    ContextMode(uint8_t id, std::string name);

    bool operator!=(ContextMode b);
    bool operator==(ContextMode b);

    static ContextMode fromValue(std::any value);

    static const ContextMode invalid;
    static const ContextMode compositor;
    static const ContextMode client;
    static const ContextMode values[3];
  };

  class Service;
  class Context : public Loggable {
    public:
      Context(ObjectArguments arguments);
      ~Context();

      std::list<std::function<std::future<std::vector<uint8_t>>(xg::Guid, std::vector<uint8_t>)>> onChannelReceive;
      std::future<std::vector<uint8_t>> channelReceive(xg::Guid engine_id, std::vector<uint8_t> message);

      std::list<std::string> getArguments();
      uv_loop_t* getLoop();
      ApplicationManifest getManifest();
      ContextMode getMode();
      template<class T> T* getSystemService();
      std::list<std::string> getSystemServiceNames();
      virtual Service* getSystemService(std::string serviceName);
      virtual std::string getPackageName();
      virtual std::string getPackageDir();
      virtual std::string getPackageDataDir();
      virtual std::string getPackageConfigDir();
    protected:
      std::map<std::string, Service*> services;
    private:
      std::list<std::string> arguments;
      Gokai::Flutter::Codecs::JSONMethodCodec method_codec;
      uv_loop_t* loop;
      ContextMode mode;
  };
}
#endif
