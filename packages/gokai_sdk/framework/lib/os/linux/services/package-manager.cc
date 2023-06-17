#include <gokai/framework/os/linux/services/package-manager.h>

using namespace Gokai::Framework::os::Linux::Services;

PackageManager::PackageManager(Gokai::ObjectArguments arguments) : Gokai::Services::PackageManager(arguments) {
  this->client = pk_client_new();
}

PackageManager::~PackageManager() {
  g_object_unref(G_OBJECT(this->client));
}