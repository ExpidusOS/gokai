#include <gokai/framework/os/linux/user/account.h>

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

ActUser* Account::getValue() {
  return this->value;
}
