#include <fmt/core.h>
#include <gokai/flutter/codecs/std.h>
#include <gokai/memory/buffer.h>
#include <gokai/memory/read-buffer.h>
#include <gokai/memory/write-buffer.h>
#include <assert.h>
#include <list>
#include <stdexcept>

using namespace Gokai::Flutter::Codecs;

enum ValueType {
  null = 0,
  boolTrue = 1,
  boolFalse = 2,
  int32 = 3,
  int64 = 4,
  largeInt = 5,
  float64 = 6,
  string = 7,
  uint8List = 8,
  int32List = 9,
  int64List = 10,
  float64List = 11,
  list = 12,
  map = 13,
  float32List = 14,
};

static size_t read_size(Gokai::Memory::ReadBuffer* buffer) {
  auto value = buffer->getUint8();
  switch (value) {
    case 254:
      return buffer->getUint16();
    case 255:
      return buffer->getUint32();
    default:
      return value;
  }
}

static void write_size(Gokai::Memory::WriteBuffer* buffer, size_t size) {
  assert(0 <= size && size <= 0xffffffff);
  if (size < 254) {
    buffer->putUint8(size);
  } else if (size <= 0xffff) {
    buffer->putUint8(254);
    buffer->putUint16(size);
  } else {
    buffer->putUint8(255);
    buffer->putUint32(size);
  }
}

static std::any value2any(Gokai::Memory::ReadBuffer* buffer) {
  auto type = (ValueType)buffer->getUint8();
  switch (type) {
    case ValueType::null:
      return nullptr;
    case ValueType::boolTrue:
      return true;
    case ValueType::boolFalse:
      return false;
    case ValueType::int32:
      return (int32_t)buffer->getUint32();
    case ValueType::int64:
      return (int32_t)buffer->getUint64();
    case ValueType::float64:
      return (double)buffer->getUint64();
    case ValueType::largeInt:
    case ValueType::string:
      {
        auto list = buffer->getUint8List(read_size(buffer));
        return std::string(list.begin(), list.end());
      }
    case ValueType::uint8List:
      {
        auto list = buffer->getUint8List(read_size(buffer));
        std::list<std::any> value;
        for (const auto& item : list) value.push_back(item);
        return value;
      }
    case ValueType::int32List:
      {
        auto list = buffer->getUint32List(read_size(buffer));
        std::list<std::any> value;
        for (const auto& item : list) value.push_back((int32_t)item);
        return value;
      }
    case ValueType::int64List:
      {
        auto list = buffer->getUint64List(read_size(buffer));
        std::list<std::any> value;
        for (const auto& item : list) value.push_back((int64_t)item);
        return value;
      }
    case ValueType::float64List:
      {
        auto list = buffer->getUint64List(read_size(buffer));
        std::list<std::any> value;
        for (const auto& item : list) value.push_back((double)item);
        return value;
      }
    case ValueType::float32List:
      {
        auto list = buffer->getUint32List(read_size(buffer));
        std::list<std::any> value;
        for (const auto& item : list) value.push_back((float)item);
        return value;
      }
    case ValueType::list:
      {
        size_t length = read_size(buffer);
        std::vector<std::any> value;
        for (size_t i = 0; i < length; i++) value.push_back(value2any(buffer));
        return value;
      }
    case ValueType::map:
      {
        size_t length = read_size(buffer);
        std::map<std::string, std::any> value;
        while ((length--) > 0) {
          auto key = std::any_cast<std::string>(value2any(buffer));
          auto val = value2any(buffer);
          value[key] = val;
        }
        return value;
      }
    default:
      throw std::runtime_error(fmt::format("Unknown type ID {}", (uint32_t)type));
  }
}

static void any2value(Gokai::Memory::WriteBuffer* buffer, std::any value) {
  if (value.type() == typeid(void*) || value.type() == typeid(void) || value.type() == typeid(nullptr)) {
    buffer->putUint8(ValueType::null);
  } else if (value.type() == typeid(bool)) {
    buffer->putUint8(std::any_cast<bool>(value) ? ValueType::boolTrue : ValueType::boolFalse);
  } else if (value.type() == typeid(int)) {
    auto num = std::any_cast<int>(value);
    if (-0x7fffffff - 1 <= num && num <= 0x7fffffff) {
      buffer->putUint8(ValueType::int32);
      buffer->putUint32(num);
    } else {
      buffer->putUint8(ValueType::int64);
      buffer->putUint64(num);
    }
  } else if (value.type() == typeid(int64_t)) {
    auto num = std::any_cast<int64_t>(value);
    if (-0x7fffffff - 1 <= num && num <= 0x7fffffff) {
      buffer->putUint8(ValueType::int32);
      buffer->putUint32(num);
    } else {
      buffer->putUint8(ValueType::int64);
      buffer->putUint64(num);
    }
  } else if (value.type() == typeid(unsigned int)) {
    auto num = std::any_cast<unsigned int>(value);
    if (-0x7fffffff - 1 <= num && num <= 0x7fffffff) {
      buffer->putUint8(ValueType::int32);
      buffer->putUint32(num);
    } else {
      buffer->putUint8(ValueType::int64);
      buffer->putUint64(num);
    }
  } else if (value.type() == typeid(uint64_t)) {
    auto num = std::any_cast<uint64_t>(value);
    if (-0x7fffffff - 1 <= num && num <= 0x7fffffff) {
      buffer->putUint8(ValueType::int32);
      buffer->putUint32(num);
    } else {
      buffer->putUint8(ValueType::int64);
      buffer->putUint64(num);
    }
  } else if (value.type() == typeid(double)) {
    buffer->putUint8(ValueType::float64);
    buffer->putUint64((uint64_t)std::any_cast<double>(value));
  } else if (value.type() == typeid(std::string)) {
    auto str = std::any_cast<std::string>(value);
    buffer->putUint8(ValueType::string);
    write_size(buffer, str.size());
    for (const auto& ch : str) buffer->putUint8(ch);
  } else if (value.type() == typeid(char*)) {
    auto str = std::any_cast<char*>(value);
    auto length = strlen(str);
    buffer->putUint8(ValueType::string);
    write_size(buffer, length);
    for (size_t i = 0; i < length; i++) buffer->putUint8(str[i]);
  } else if (value.type() == typeid(const char*)) {
    auto str = std::any_cast<const char*>(value);
    auto length = strlen(str);
    buffer->putUint8(ValueType::string);
    write_size(buffer, length);
    for (size_t i = 0; i < length; i++) buffer->putUint8(str[i]);
  } else if (value.type() == typeid(std::list<std::any>)) {
    auto list = std::any_cast<std::list<std::any>>(value);
    buffer->putUint8(ValueType::list);
    write_size(buffer, list.size());
    for (const auto& item : list) any2value(buffer, item);
  } else if (value.type() == typeid(std::map<std::string, std::any>)) {
    auto map = std::any_cast<std::map<std::string, std::any>>(value);
    buffer->putUint8(ValueType::map);
    write_size(buffer, map.size());
    for (const auto& entry : map) {
      any2value(buffer, entry.first);
      any2value(buffer, entry.second);
    }
  } else {
    throw std::runtime_error(fmt::format("Invalid type decoded from: {}", value.type().name()));
  }
}

StandardMessageCodec::StandardMessageCodec(Gokai::ObjectArguments arguments) : Gokai::Flutter::MessageCodec<std::any>(arguments) {}

std::any StandardMessageCodec::decodeMessage(std::vector<uint8_t> message) {
  auto buffer = Gokai::Memory::ReadBuffer(Gokai::ObjectArguments({
    { "buffer", message },
    { "position", 0 },
  }));
  return value2any(&buffer);
}

std::vector<uint8_t> StandardMessageCodec::encodeMessage(std::any message) {
  auto buffer = Gokai::Memory::WriteBuffer(Gokai::ObjectArguments({}));
  any2value(&buffer, message);
  return buffer.toVector();
}

StandardMethodCodec::StandardMethodCodec(Gokai::ObjectArguments arguments) : Gokai::Flutter::MethodCodec(arguments), message_codec{arguments} {}

std::any StandardMethodCodec::decodeEnvelope(std::vector<uint8_t> envelope) {
  auto buffer = Gokai::Memory::ReadBuffer(Gokai::ObjectArguments({
    { "buffer", envelope },
    { "position", 0 },
  }));

  if (buffer.getUint8() == 0) {
    return value2any(&buffer);
  }

  auto error_code = value2any(&buffer);
  auto error_message = value2any(&buffer);
  auto error_details = value2any(&buffer);
  auto error_stack = buffer.hasRemaining() ? std::any_cast<std::string>(value2any(&buffer)) : "";

  if (error_code.type() == typeid(std::string) && error_message.type() == typeid(std::string) && !buffer.hasRemaining()) {
    throw std::runtime_error(fmt::format("{}: {}", std::any_cast<std::string>(error_code), std::any_cast<std::string>(error_message)));
  } else if (error_code.type() == typeid(std::string) && error_message.type() == typeid(nullptr) && !buffer.hasRemaining()) {
    throw std::runtime_error(fmt::format("{}", std::any_cast<std::string>(error_code)));
  } else {
    throw std::runtime_error("Invalid envelope");
  }
}

std::vector<uint8_t> StandardMethodCodec::encodeErrorEnvelope(std::string code, std::string message, std::any details) {
  auto buffer = Gokai::Memory::WriteBuffer(Gokai::ObjectArguments({}));
  buffer.putUint8(1);

  any2value(&buffer, code);
  any2value(&buffer, message);
  any2value(&buffer, details);
  return buffer.toVector();
}

Gokai::Flutter::MethodCall StandardMethodCodec::decodeMethodCall(std::vector<uint8_t> call) {
  auto buffer = Gokai::Memory::ReadBuffer(Gokai::ObjectArguments({
    { "buffer", call },
    { "position", 0 },
  }));

  auto method = value2any(&buffer);
  auto arguments = value2any(&buffer);

  if (method.type() == typeid(std::string) && !buffer.hasRemaining()) {
    return Gokai::Flutter::MethodCall(std::any_cast<std::string>(method), arguments);
  } else {
    throw std::runtime_error("Invalid method call");
  }
}

std::vector<uint8_t> StandardMethodCodec::encodeMethodCall(MethodCall call) {
  auto buffer = Gokai::Memory::WriteBuffer(Gokai::ObjectArguments({}));
  any2value(&buffer, call.method);
  any2value(&buffer, call.arguments);
  return buffer.toVector();
}

std::vector<uint8_t> StandardMethodCodec::encodeSuccessEnvelope(std::any result) {
  auto buffer = Gokai::Memory::WriteBuffer(Gokai::ObjectArguments({}));
  buffer.putUint8(0);
  any2value(&buffer, result);
  return buffer.toVector();
}
