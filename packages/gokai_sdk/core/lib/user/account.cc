#include <gokai/user/account.h>

using namespace Gokai::User;

Account::Account(Gokai::ObjectArguments arguments) : Object(arguments) {}

ID Account::getId() {
  return ID(-1);
}

std::locale Account::getLanguage() {
  return std::locale("");
}
