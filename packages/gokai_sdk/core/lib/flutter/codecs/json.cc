#include <fmt/core.h>
#include <gokai/flutter/codecs/json.h>
#include <json/json.h>
#include <list>
#include <stdexcept>

using namespace Gokai::Flutter::Codecs;

static std::any jsonvalue2any(Json::Value value) {
  if (value.isNull()) {
    return std::make_any<void*>(nullptr);
  } else if (value.isBool()) {
    return std::make_any<bool>(value.asBool());
  } else if (value.isInt()) {
    return std::make_any<int>(value.asInt());
  } else if (value.isInt64()) {
    return std::make_any<int64_t>(value.asInt64());
  } else if (value.isUInt()) {
    return std::make_any<unsigned int>(value.asUInt());
  } else if (value.isUInt64()) {
    return std::make_any<uint64_t>(value.asUInt64());
  } else if (value.isDouble()) {
    return std::make_any<double>(value.asDouble());
  } else if (value.isString()) {
    return std::make_any<std::string>(value.asString());
  } else if (value.isArray()) {
    std::list<std::any> list;
    for (const auto& item : value) {
      list.push_back(jsonvalue2any(item));
    }
    return list;
  } else if (value.isObject()) {
    std::map<std::string, std::any> map;
    for (const auto& key : value.getMemberNames()) {
      map[key] = jsonvalue2any(value[key]);
    }
    return map;
  }

  throw std::runtime_error(fmt::format("Invalid type decoded from: {}", value.toStyledString()));
}

static Json::Value any2jsonvalue(std::any value) {
  if (value.type() == typeid(void*)) {
    return Json::Value();
  } else if (value.type() == typeid(bool)) {
    return Json::Value(std::any_cast<bool>(value));
  } else if (value.type() == typeid(int)) {
    return Json::Value(std::any_cast<int>(value));
  } else if (value.type() == typeid(int64_t)) {
    return Json::Value(std::any_cast<int64_t>(value));
  } else if (value.type() == typeid(unsigned int)) {
    return Json::Value(std::any_cast<unsigned int>(value));
  } else if (value.type() == typeid(uint64_t)) {
    return Json::Value(std::any_cast<uint64_t>(value));
  } else if (value.type() == typeid(double)) {
    return Json::Value(std::any_cast<double>(value));
  } else if (value.type() == typeid(std::string)) {
    return Json::Value(std::any_cast<std::string>(value));
  } else if (value.type() == typeid(std::list<std::any>)) {
    auto value_list = std::any_cast<std::list<std::any>>(value);
    Json::Value list;
    for (const auto& item : value_list) {
      list.append(any2jsonvalue(item));
    }
    return list;
  } else if (value.type() == typeid(std::map<std::string, std::any>)) {
    auto value_map = std::any_cast<std::map<std::string, std::any>>(value);
    Json::Value map;
    for (const auto& entry : value_map) {
      map[entry.first] = any2jsonvalue(entry.second);
    }
    return map;
  }

  throw std::runtime_error(fmt::format("Invalid type decoded from: {}", value.type().name()));
}

JSONMessageCodec::JSONMessageCodec(Gokai::ObjectArguments arguments) : Gokai::Flutter::MessageCodec<std::any>(arguments) {}

std::any JSONMessageCodec::decodeMessage(std::vector<uint8_t> message) {
  std::string message_string(message.begin(), message.end());
  auto message_string_length = message_string.length();

  JSONCPP_STRING err;
  Json::Value root;
  Json::CharReaderBuilder builder;
  const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
  if (!reader->parse(message_string.c_str(), message_string.c_str() + message_string_length, &root, &err)) {
    throw std::runtime_error(fmt::format("Failed to parse JSON string: {}", err));
  }

  return jsonvalue2any(root);
}

std::vector<uint8_t> JSONMessageCodec::encodeMessage(std::any message) {
  Json::StreamWriterBuilder builder;
  auto value = Json::writeString(builder, any2jsonvalue(message));
  return std::vector<uint8_t>(value.begin(), value.end());
}

JSONMethodCodec::JSONMethodCodec(Gokai::ObjectArguments arguments) : Gokai::Flutter::MethodCodec(arguments), message_codec{arguments} {}

std::any JSONMethodCodec::decodeEnvelope(std::vector<uint8_t> envelope) {
  auto decoded_any = this->message_codec.decodeMessage(envelope);

  if (decoded_any.type() != typeid(std::list<std::any>)) {
    throw std::runtime_error(fmt::format("Expected envelope list, got {}", decoded_any.type().name()));
  }

  auto decoded = std::any_cast<std::list<std::any>>(decoded_any);
  std::vector<std::any> decoded_vec(decoded.begin(), decoded.end());
  auto decoded_length = decoded.size();

  if (decoded_length == 1) {
    return decoded_vec[0];
  }

  if (decoded_length == 3
      && decoded_vec[0].type() == typeid(std::string)
      && (decoded_vec[1].type() == typeid(void*) || decoded_vec[1].type() == typeid(std::string))) {
    auto code = std::any_cast<std::string>(decoded_vec[0]);
    auto message = decoded_vec[1].type() == typeid(void*) ? std::string("") : std::any_cast<std::string>(decoded_vec[1]);
    throw std::runtime_error(fmt::format("{}: {}", code, message));
  }

  if (decoded_length == 4
      && decoded_vec[0].type() == typeid(std::string)
      && (decoded_vec[1].type() == typeid(void*) || decoded_vec[1].type() == typeid(std::string))
      && (decoded_vec[3].type() == typeid(void*) || decoded_vec[3].type() == typeid(std::string))) {
    auto code = std::any_cast<std::string>(decoded_vec[0]);
    auto message = decoded_vec[1].type() == typeid(void*) ? std::string("") : std::any_cast<std::string>(decoded_vec[1]);
    auto stacktrace = decoded_vec[3].type() == typeid(void*) ? std::string("") : std::any_cast<std::string>(decoded_vec[3]);
    throw std::runtime_error(fmt::format("{}: {}\n{}", code, message, stacktrace));
  }

  std::string envelope_str(envelope.begin(), envelope.end());
  throw std::runtime_error(fmt::format("Invalid envelope: {}", envelope_str));
}

std::vector<uint8_t> JSONMethodCodec::encodeErrorEnvelope(std::string code, std::string message, std::any details) {
  std::list<std::any> value;
  value.push_back(code);
  value.push_back(message);
  value.push_back(details);
  return this->message_codec.encodeMessage(value);
}

Gokai::Flutter::MethodCall JSONMethodCodec::decodeMethodCall(std::vector<uint8_t> call) {
  auto decoded_any = this->message_codec.decodeMessage(call);

  if (decoded_any.type() != typeid(std::map<std::string, std::any>)) {
    throw std::runtime_error(fmt::format("Expected method call map, got {}", decoded_any.type().name()));
  }

  auto decoded = std::any_cast<std::map<std::string, std::any>>(decoded_any);
  auto method = decoded["method"];
  auto arguments = decoded["args"];

  if (method.type() == typeid(std::string)) {
    auto value = Gokai::Flutter::MethodCall();
    value.method = std::any_cast<std::string>(method);
    value.arguments = arguments;
    return value;
  }

  std::string call_str(call.begin(), call.end());
  throw std::runtime_error(fmt::format("Invalid method call: {}", call_str));
}

std::vector<uint8_t> JSONMethodCodec::encodeMethodCall(MethodCall call) {
  std::map<std::string, std::any> value;
  value["method"] = call.method;
  value["args"] = call.arguments;
  return this->message_codec.encodeMessage(value);
}

std::vector<uint8_t> JSONMethodCodec::encodeSuccessEnvelope(std::any result) {
  std::list<std::any> value;
  value.push_back(result);
  return this->message_codec.encodeMessage(value);
}