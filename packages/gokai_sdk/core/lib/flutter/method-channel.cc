#include <fmt/core.h>
#include <gokai/flutter/codecs/json.h>
#include <gokai/flutter/method-channel.h>
#include <stdexcept>

#define TAG "Gokai::Flutter::MethodChannel"

using namespace Gokai::Flutter;

MethodChannel::MethodChannel(Gokai::ObjectArguments arguments)
  : Loggable(fmt::format("{}#{}", TAG, std::any_cast<std::string>(arguments.get("name"))).c_str(), arguments),
    engine{std::any_cast<std::shared_ptr<Engine>>(arguments.get("engine"))},
    name{std::any_cast<std::string>(arguments.get("name"))},
    codec{arguments},
    onResponseFunc{[this](std::vector<uint8_t> in) {
      if (this->handler == nullptr) return std::vector<uint8_t>();

      auto call = this->codec.decodeMethodCall(in);
      try {
        auto value = this->handler(call);
        return this->codec.encodeSuccessEnvelope(value);
      } catch (const std::exception& ex) {
        return this->codec.encodeErrorEnvelope(this->name, ex.what(), std::make_any<void*>(nullptr));
      }
    }} {
  if (arguments.has("codec")) {
    this->codec = std::any_cast<MethodCodec>(arguments.get("codec"));
  } else {
    this->codec = Codecs::JSONMethodCodec(arguments);
  }

  auto find = this->engine->method_channel_handlers.find(this->name);
  if (find == this->engine->method_channel_handlers.end()) {
    this->engine->method_channel_handlers[this->name] = std::list<std::function<std::vector<uint8_t>(std::vector<uint8_t>)>>();
  }

  this->engine->method_channel_handlers[this->name].emplace_back(this->onResponseFunc);
}

MethodChannel::~MethodChannel() {
  auto list = this->engine->method_channel_handlers[this->name];
  for (auto it = list.begin(); it != list.end(); it++) {
    if (it->target_type() == this->onResponseFunc.target_type()) {
      this->engine->method_channel_handlers[this->name].erase(it);
      break;
    }
  }
}

std::future<std::any> MethodChannel::invokeMethod(std::string name, std::any arguments) {
  auto call = MethodCall();
  call.method = name;
  call.arguments = arguments;

  auto encoded_call = this->codec.encodeMethodCall(call);
  return std::async(std::launch::async, [this, encoded_call]() {
    auto future = this->engine->send(this->name, encoded_call);
    future.wait();
    return this->codec.decodeEnvelope(future.get());
  });
}