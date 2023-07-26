#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/logging.h>
#include <list>
#include <memory>
#include <string>

namespace Gokai {
  class Context;
  class ServiceChannel : public Loggable {
    public:
      ServiceChannel(ObjectArguments arguments);

      std::list<std::function<std::vector<uint8_t>(xg::Guid, std::string, std::vector<uint8_t>)>> onReceive;

      std::list<std::string> getNames();
      bool accepts(std::string channel);
      std::vector<uint8_t> receive(xg::Guid engine_id, std::string channel, std::vector<uint8_t> message);
    protected:
      std::shared_ptr<Gokai::Context> context;
    private:
      std::list<std::string> names;
  };
}
#endif
