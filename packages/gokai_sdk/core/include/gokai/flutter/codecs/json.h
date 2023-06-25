#pragma once

#ifdef __cplusplus
#include <gokai/flutter/message-codec.h>
#include <gokai/flutter/method-codec.h>

namespace Gokai {
  namespace Flutter {
    namespace Codecs {
      class JSONMessageCodec : public Gokai::Flutter::MessageCodec<std::any> {
        public:
          JSONMessageCodec(Gokai::ObjectArguments arguments);

          std::any decodeMessage(std::vector<uint8_t> message) override;
          std::vector<uint8_t> encodeMessage(std::any message) override;
      };

      class JSONMethodCodec : public Gokai::Flutter::MethodCodec {
        public:
          JSONMethodCodec(Gokai::ObjectArguments arguments);

          std::any decodeEnvelope(std::vector<uint8_t> envelope) override;
          Gokai::Flutter::MethodCall decodeMethodCall(std::vector<uint8_t> call) override;
          std::vector<uint8_t> encodeErrorEnvelope(std::string code, std::string message, std::any details) override;
          std::vector<uint8_t> encodeMethodCall(MethodCall call) override;
          std::vector<uint8_t> encodeSuccessEnvelope(std::any result) override;
        private:
          JSONMessageCodec message_codec;
      };
    }
  }
}
#endif