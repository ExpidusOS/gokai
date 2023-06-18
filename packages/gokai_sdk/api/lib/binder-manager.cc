#include <gokai/api/binder-manager.h>
#include <gokai/values/bool.h>
#include <gokai/values/string.h>

using namespace Gokai::API;

BinderManager::BinderManager(Gokai::ObjectArguments arguments) : Object(arguments) {
  if (arguments.has("portable")) {
    this->is_portable = static_cast<Gokai::Values::Boolean*>(arguments.getPointed("portable"))->getValue();
  }

  if (arguments.has("default")) {
    this->binder_default = this->load(static_cast<Gokai::Values::String*>(arguments.getPointed("default"))->getValue());
  }
}

BinderManager::~BinderManager() {
  if (!this->isCached(this->binder_default)) {
    delete this->binder_default;
  }

  for (auto entry : this->binder_cache) delete entry.second;
}

bool BinderManager::isCached(std::string name) {
  auto find = this->binder_cache.find(name);
  return find != this->binder_cache.end();
}

bool BinderManager::isCached(Binder* binder) {
  return this->isCached(binder->getPath());
}

Binder* BinderManager::getCached(std::string name) {
  for (auto entry : this->binder_cache) {
    if (entry.first.compare(name) == 0) return entry.second;
  }
  return nullptr;
}

Binder* BinderManager::load(std::string name) {
  return this->getCached(name);
}

Binder* BinderManager::getDefault() {
  if (this->binder_default == nullptr) {
    auto first = this->binder_cache.begin();
    if (first != this->binder_cache.end()) return first->second;
  }
  return this->binder_default;
}

std::map<std::string, Binder*> BinderManager::getAll() {
  return this->binder_cache;
}