#pragma once

#ifdef __cplusplus
#include <gokai/flutter/method-call.h>
#include <gokai/object.h>
#include <flutter_embedder.h>
#include <stdexcept>
#include <vector>

namespace Gokai {
  namespace Flutter {
    template<typename T> class MessageCodec : public Gokai::Object {
      public:
        MessageCodec(Gokai::ObjectArguments arguments) : Object(arguments) {}

        virtual T decodeMessage(std::vector<uint8_t> message) {
          throw std::runtime_error("Not implemented: MessageCodec<T>::decodeMessage()");
        }

        virtual std::vector<uint8_t> encodeMessage(T message) {
          throw std::runtime_error("Not implemented: MessageCodec<T>::encodeMessage()");
        }
    };
  }
}
#endif