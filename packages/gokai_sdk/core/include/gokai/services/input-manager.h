#pragma once

#ifdef __cplusplus
#include <gokai/input/base.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/service.h>
#include <string>

namespace Gokai {
  namespace Services {
    class InputManager : public Gokai::Service, public Gokai::Loggable {
      public:
        InputManager(Gokai::ObjectArguments arguments);

        virtual std::list<std::string> getNames();
        virtual std::shared_ptr<Gokai::Input::Base> get(std::string name);

        static const std::string SERVICE_NAME;
    };
  }
}
#endif