#include <gokai/framework/os/linux/user/account.h>
#include <spdlog/spdlog.h>

using namespace Gokai::Framework::os::Linux::User;

Account::Account(Gokai::ObjectArguments arguments) : Gokai::User::Account(arguments) {
  this->value = ACT_USER(g_object_ref(G_OBJECT(std::any_cast<ActUser*>(arguments.get("value")))));
}

Account::~Account() {
  g_clear_object(&this->value);
}

Gokai::User::ID Account::getId() {
  return Gokai::User::ID(act_user_get_uid(this->value));
}

std::locale Account::getLanguage() {
  return std::locale(act_user_get_language(this->value));
}

std::string Account::getDisplayName() {
  return std::string(act_user_get_real_name(this->value));
}

std::string Account::getPicture() {
  return std::string(act_user_get_icon_file(this->value));
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
