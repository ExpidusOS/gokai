#include <gokai/services/account-manager.h>
#include <gokai/services/engine-manager.h>

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

    if (call.method.compare("getCurrentId") == 0) {
      return this->method_codec.encodeSuccessEnvelope(this->getCurrentId().toAny());
    }

    if (call.method.compare("getIds") == 0) {
      auto ids = this->getIds();
      std::list<std::any> list;
      for (auto& id : ids) list.push_back(id.toAny());
      return this->method_codec.encodeSuccessEnvelope(list);
    }

    if (call.method.compare("getLanguage") == 0) {
      auto id = Gokai::User::ID(call.arguments);
      auto user = this->get(id);
      if (user == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("User does not exist"), std::make_any<void*>(nullptr));
      }

      auto lang = user->getLanguage();
      return this->method_codec.encodeSuccessEnvelope(lang.name());
    }

    if (call.method.compare("getDisplayName") == 0) {
      auto id = Gokai::User::ID(call.arguments);
      auto user = this->get(id);
      if (user == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("User does not exist"), std::make_any<void*>(nullptr));
      }

      return this->method_codec.encodeSuccessEnvelope(user->getDisplayName());
    }

    if (call.method.compare("getPicture") == 0) {
      auto id = Gokai::User::ID(call.arguments);
      auto user = this->get(id);
      if (user == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("User does not exist"), std::make_any<void*>(nullptr));
      }

      auto value = user->getPicture();
      return this->method_codec.encodeSuccessEnvelope(value.length() == 0 ? std::any(nullptr) : std::any(value));
    }

    if (call.method.compare("isSystem") == 0) {
      auto id = Gokai::User::ID(call.arguments);
      auto user = this->get(id);
      if (user == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("User does not exist"), std::make_any<void*>(nullptr));
      }

      return this->method_codec.encodeSuccessEnvelope(user->isSystem());
    }

    if (call.method.compare("isAdministrator") == 0) {
      auto id = Gokai::User::ID(call.arguments);
      auto user = this->get(id);
      if (user == nullptr) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("User does not exist"), std::make_any<void*>(nullptr));
      }

      return this->method_codec.encodeSuccessEnvelope(user->isAdministrator());
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });

  this->changed.push_back([this]() {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "changed";
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });
}

std::shared_ptr<Gokai::ServiceChannel> AccountManager::getServiceChannel() {
  return this->service_channel;
}

Gokai::User::ID AccountManager::getCurrentId() {
  return Gokai::User::ID(-1);
}

std::list<Gokai::User::ID> AccountManager::getIds() {
  return std::list<Gokai::User::ID>();
}

Gokai::User::Account* AccountManager::get(Gokai::User::ID id) {
  return nullptr;
}

Gokai::User::Account* AccountManager::get(xg::Guid guid) {
  return this->get(Gokai::User::ID(guid));
}

Gokai::User::Account* AccountManager::get(uint32_t uid) {
  return this->get(Gokai::User::ID(uid));
}

const std::string AccountManager::SERVICE_NAME = "AccountManager";
