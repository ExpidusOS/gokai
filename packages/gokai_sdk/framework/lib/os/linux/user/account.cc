#include <gokai/framework/os/linux/user/account.h>
#include <spdlog/spdlog.h>

using namespace Gokai::Framework::os::Linux::User;

Account::Account(Gokai::ObjectArguments arguments) : Gokai::User::Account(arguments) {
  this->value = ACT_USER(g_object_ref(G_OBJECT(std::any_cast<ActUser*>(arguments.get("value")))));
  this->changed_id = g_signal_connect(this->value, "changed", G_CALLBACK(Account::changed), this);

  if (!act_user_is_loaded(this->value)) spdlog::warn("User {} is not loaded", act_user_get_uid(this->value));
}

Account::~Account() {
  g_signal_handler_disconnect(this->value, this->changed_id);
  g_clear_object(&this->value);
}

Gokai::User::ID Account::getId() {
  return Gokai::User::ID(act_user_get_uid(this->value));
}

std::locale Account::getLanguage() {
  auto value = act_user_get_language(this->value);
  return std::locale(value == nullptr ? "" : value);
}

std::string Account::getDisplayName() {
  auto value = act_user_get_real_name(this->value);
  return std::string(value == nullptr ? "" : value);
}

std::string Account::getPicture() {
  auto value = act_user_get_icon_file(this->value);
  return std::string(value == nullptr ? "" : value);
}

std::string Account::getHome() {
  auto value = act_user_get_home_dir(this->value);
  return std::string(value == nullptr ? "" : value);
}

bool Account::isSystem() {
  return act_user_is_system_account(this->value);
}

bool Account::isAdministrator() {
  return act_user_get_account_type(this->value) == ACT_USER_ACCOUNT_TYPE_ADMINISTRATOR;
}

ActUser* Account::getValue() {
  return this->value;
}

void Account::changed(ActUser* user, gpointer data) {
  auto self = reinterpret_cast<Account*>(data);
  for (const auto& func : self->onChange) func();
}
