#include <gokai/services/engine-manager.h>
#include <gokai/user/account.h>
#include <assert.h>

using namespace Gokai::User;

Account::Account(Gokai::ObjectArguments arguments) : Object(arguments) {
  auto context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));
  assert(context != nullptr);
  this->context = context;

  this->onChange.push_back([this]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "change";
    call.arguments = this->getId();
    engine_manager->sendAll("Gokai::Services::AccountManager", engine_manager->method_codec.encodeMethodCall(call));
  });
}

ID Account::getId() {
  return ID(-1);
}

std::locale Account::getLanguage() {
  return std::locale("");
}

std::string Account::getDisplayName() {
  return std::string();
}

std::string Account::getPicture() {
  return std::string();
}

bool Account::isSystem() {
  return false;
}

bool Account::isAdministrator() {
  return false;
}
