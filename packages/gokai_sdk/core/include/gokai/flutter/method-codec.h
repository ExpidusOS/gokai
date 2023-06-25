#pragma once

#ifdef __cplusplus
#include <gokai/flutter/method-call.h>
#include <gokai/object.h>
#include <flutter_embedder.h>
#include <vector>

namespace Gokai {
  namespace Flutter {
    class MethodCodec : public Gokai::Object {
      public:
        MethodCodec(Gokai::ObjectArguments arguments);

        virtual std::any decodeEnvelope(std::vector<uint8_t> envelope);
        virtual MethodCall decodeMethodCall(std::vector<uint8_t> call);

        virtual std::vector<uint8_t> encodeErrorEnvelope(std::string code, std::string message, std::any details);
        std::vector<uint8_t> encodeErrorEnvelope(std::string code, std::string message);
        std::vector<uint8_t> encodeErrorEnvelope(std::string code);

        virtual std::vector<uint8_t> encodeMethodCall(MethodCall call);
        virtual std::vector<uint8_t> encodeSuccessEnvelope(std::any result);
    };
  }
}
#endif