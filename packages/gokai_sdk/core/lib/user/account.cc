#include <gokai/user/account.h>

using namespace Gokai::User;

Account::Account(Gokai::ObjectArguments arguments) : Object(arguments) {}

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
