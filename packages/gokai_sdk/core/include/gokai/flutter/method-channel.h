#pragma once

#ifdef __cplusplus
#include <gokai/flutter/engine.h>
#include <gokai/flutter/message-codec.h>
#include <gokai/flutter/method-codec.h>
#include <gokai/logging.h>
#include <future>

namespace Gokai {
  namespace Flutter {
    class MethodChannel : public Gokai::Loggable {
      public:
        MethodChannel(Gokai::ObjectArguments arguments);

        MethodCodec codec;
        std::string name;
        std::list<std::function<void(std::any)>> handlers;

        std::promise<std::any> invokeMethod(std::string name, std::any arguments);
      private:
        std::shared_ptr<Engine> engine;
    };
  }
}
#endif