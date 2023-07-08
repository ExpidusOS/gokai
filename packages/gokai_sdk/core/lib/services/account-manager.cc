#include <gokai/services/account-manager.h>

#define TAG "Gokai::Services::AccountManager"

using namespace Gokai::Services;

AccountManager::AccountManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);

    if (call.method.compare("getIds") == 0) {
      auto ids = this->getIds();
      std::list<std::any> list;
      for (auto& id : ids) list.push_back(id.toAny());
      return this->method_codec.encodeSuccessEnvelope(list);
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });
}

std::shared_ptr<Gokai::ServiceChannel> AccountManager::getServiceChannel() {
  return this->service_channel;
}

std::list<Gokai::User::ID> AccountManager::getIds() {
  return std::list<Gokai::User::ID>();
}

const std::string AccountManager::SERVICE_NAME = "AccountManager";
