#include <gokai/object-arguments.h>
#include <assert.h>
#include <stdexcept>

using namespace Gokai;

ObjectArguments::ObjectArguments(std::map<std::string, std::any> arguments, TypeSafePolicy policy) {
  this->arguments = arguments;
  this->policy = policy;
}

ObjectArguments::ObjectArguments(std::map<std::string, std::any> arguments) : ObjectArguments(arguments, GOKAI_TYPE_SAFE_POLICY_EXCEPTION) {}

bool ObjectArguments::has(std::string name) {
  return this->arguments.count(name) > 0;
}

std::any ObjectArguments::get(std::string name) {
  switch (this->policy) {
    case GOKAI_TYPE_SAFE_POLICY_ASSERT:
      return this->getAssert(name);
    case GOKAI_TYPE_SAFE_POLICY_EXCEPTION:
      return this->getException(name);
    case GOKAI_TYPE_SAFE_POLICY_NONE:
      return this->getUnsafe(name);
  }

  throw std::runtime_error("An invalid type-safe policy was set");
}

std::any ObjectArguments::getAssert(std::string name) {
  auto pair = this->arguments.find(name);
  assert(pair != this->arguments.end());
  return pair->second;
}

std::any ObjectArguments::getException(std::string name) {
  auto pair = this->arguments.find(name);
  if (pair == this->arguments.end()) {
    throw std::invalid_argument("Argument \"" + name + "\" does not exist");
  }

  return pair->second;
}

std::any ObjectArguments::getUnsafe(std::string name) {
  return this->arguments.find(name)->second;
}