#include <gokai/framework/os/linux/services/account-manager.h>
#include <gokai/framework/os/linux/user/account.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::Services;

AccountManager::AccountManager(Gokai::ObjectArguments arguments) : Gokai::Services::AccountManager(arguments) {
  this->manager = ACT_USER_MANAGER(g_object_new(ACT_TYPE_USER_MANAGER, nullptr));

  if (act_user_manager_no_service(this->manager)) {
    throw std::runtime_error("AccountsService is not available.");
  }

  this->user_changed_id = g_signal_connect(this->manager, "user-changed", G_CALLBACK(AccountManager::user_changed), this);
}

AccountManager::~AccountManager() {
  g_signal_handler_disconnect(this->manager, this->user_changed_id);

  g_clear_object(&this->manager);
}

Gokai::User::ID AccountManager::getCurrentId() {
  return Gokai::User::ID(geteuid());
}

std::list<Gokai::User::ID> AccountManager::getIds() {
  std::list<Gokai::User::ID> list;

  GSList* head = act_user_manager_list_users(this->manager);
  for (GSList* value = head; value != nullptr; value = value->next) {
    ActUser* user = ACT_USER(value->data);
    list.push_back(Gokai::User::ID(act_user_get_uid(user)));
  }
  g_slist_free(head);
  return list;
}

Gokai::User::Account* AccountManager::get(Gokai::User::ID id) {
  auto find = this->cache.find(id);
  if (find == this->cache.end()) {
    if (id.type != Gokai::User::uid) {
      throw std::runtime_error("Invalid user ID type");
    }

    if (this->cache.size() == 0) {
      g_slist_free(act_user_manager_list_users(this->manager));
    }

    auto user = act_user_manager_get_user_by_id(this->manager, id.data.uid);
    if (user == nullptr) return nullptr;

    auto account = new Gokai::Framework::os::Linux::User::Account(Gokai::ObjectArguments({
      { "context", this->context },
      { "value", user }
    }));
    this->cache[id] = account;
    return account;
  }
  return find->second;
}

void AccountManager::user_changed(ActUser* user, gpointer user_data) {
  auto self = reinterpret_cast<AccountManager*>(user_data);
  auto ids = self->getIds();

  self->cache.clear();
  for (const auto& func : self->changed) func();
}
