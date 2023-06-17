#include <gokai/context.h>
#include <gokai/services/display-manager.h>
#include <gokai/services/window-manager.h>
#include <type_traits>

using namespace Gokai;

template<class T> T* Context::getSystemService() {
  if (std::is_same<T, Services::DisplayManager>::value) {
    return getSystemService(Services::DisplayManager::SERVICE_NAME);
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