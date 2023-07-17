#include <gokai/services/engine-manager.h>
#include <gokai/services/window-manager.h>
#include <gokai/view/window.h>

using namespace Gokai::View;

Window::Window(Gokai::ObjectArguments arguments) : Object(arguments) {
  this->context = std::any_cast<std::shared_ptr<Context>>(arguments.get("context"));

  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  } else {
    this->id = xg::newGuid();
  }

  auto window_manager = reinterpret_cast<Gokai::Services::WindowManager*>(this->context->getSystemService(Gokai::Services::WindowManager::SERVICE_NAME));

  this->onActive.push_back([this, window_manager]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "active";
    call.arguments = this->id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", window_manager->method_codec.encodeMethodCall(call));
  });

  this->onMapped.push_back([this, window_manager]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "mapped";
    call.arguments = this->id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", window_manager->method_codec.encodeMethodCall(call));
  });

  this->onCommit.push_back([this, window_manager]() {
    auto engine_manager = reinterpret_cast<Gokai::Services::EngineManager*>(this->context->getSystemService(Gokai::Services::EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "commit";
    call.arguments = this->id.str();
    engine_manager->sendAll("Gokai::Services::WindowManager", window_manager->method_codec.encodeMethodCall(call));
  });
}

bool Window::isToplevel() {
  return false;
}

std::string Window::getRole() {
  return "";
}

std::list<xg::Guid> Window::getChildrenAbove() {
  return std::list<xg::Guid>();
}

std::list<xg::Guid> Window::getChildrenBelow() {
  return std::list<xg::Guid>();
}

std::string Window::getDisplayName() {
  return std::string();
}

bool Window::isMapped() {
  return this->is_mapped;
}

void Window::setMapped(bool value) {
  this->is_mapped = value;
  for (const auto& func : this->onMapped) func();
}

bool Window::isActive() {
  return this->is_active;
}

void Window::setActive(bool value) {
  this->is_active = value;
  for (const auto& func : this->onActive) func();
}

xg::Guid Window::getId() {
  return this->id;
}

URect Window::getRect() {
  return this->rect;
}

void Window::setRect(URect rect) {
  this->rect = rect;
}

bool Window::hasDecorations() {
  return false;
}

bool Window::hasTexture() {
  return false;
}

int64_t Window::getTextureId() {
  return 0;
}

std::shared_ptr<Gokai::Graphics::Texture> Window::getTexture() {
  return nullptr;
}

std::string Window::getTitle() {
  return "";
}
