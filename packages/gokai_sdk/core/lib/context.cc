#include <gokai/context.h>
#include <gokai/services/display-manager.h>
#include <gokai/services/package-manager.h>
#include <gokai/services/window-manager.h>
#include <assert.h>
#include <type_traits>

using namespace Gokai;

Context::Context(ObjectArguments arguments) : Object(arguments) {
  this->loop = reinterpret_cast<uv_loop_t*>(malloc(sizeof (uv_loop_t)));
  assert(this->loop != nullptr);
  uv_loop_init(this->loop);
}

Context::~Context() {
  uv_loop_close(this->loop);
  free(this->loop);
}

template<class T> T* Context::getSystemService() {
  if (std::is_same<T, Services::DisplayManager>::value) {
    return getSystemService(Services::DisplayManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::PackageManager>::value) {
    return getSystemService(Services::PackageManager::SERVICE_NAME);
  }

  if (std::is_same<T, Services::WindowManager>::value) {
    return getSystemService(Services::WindowManager::SERVICE_NAME);
  }
  return nullptr;
}

Service* Context::getSystemService(std::string serviceName) {
  return nullptr;
}

std::string Context::getPackageName() {
  return nullptr;
}

std::string Context::getPackageDir() {
  return nullptr;
}