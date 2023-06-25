#include <gokai/flutter/method-codec.h>

using namespace Gokai::Flutter;

MethodCodec::MethodCodec(Gokai::ObjectArguments arguments) : Object(arguments) {}

std::any MethodCodec::decodeEnvelope(std::vector<uint8_t> envelope) {
  return std::make_any<void*>(nullptr);
}

MethodCall MethodCodec::decodeMethodCall(std::vector<uint8_t> call) {
  return MethodCall();
}

std::vector<uint8_t> MethodCodec::encodeErrorEnvelope(std::string code, std::string message, std::any details) {
  return std::vector<uint8_t>();
}

std::vector<uint8_t> MethodCodec::encodeErrorEnvelope(std::string code, std::string message) {
  return this->encodeErrorEnvelope(code, message, std::make_any<void*>(nullptr));
}

std::vector<uint8_t> MethodCodec::encodeErrorEnvelope(std::string code) {
  return this->encodeErrorEnvelope(code, std::string());
}

std::vector<uint8_t> MethodCodec::encodeMethodCall(MethodCall call) {
  return std::vector<uint8_t>();
}

std::vector<uint8_t> MethodCodec::encodeSuccessEnvelope(std::any result) {
  return std::vector<uint8_t>();
}