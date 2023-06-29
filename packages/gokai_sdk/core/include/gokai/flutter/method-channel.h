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
        ~MethodChannel();

        MethodCodec codec;
        std::string name;
        std::function<std::any(MethodCall)> handler;

        std::future<std::any> invokeMethod(std::string name, std::any arguments);
      private:
        std::function<std::vector<uint8_t>(std::vector<uint8_t>)> onResponseFunc;
        std::shared_ptr<Engine> engine;
    };
  }
}
#endif