#include <gokai/api/os/linux/binder-manager.h>
#include <gokai/flutter/common.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <assert.h>

extern "C" FLUTTER_PLUGIN_EXPORT int main(int argc, char** argv) {
  spdlog::cfg::load_env_levels();

  auto manager = new Gokai::API::os::Linux::BinderManager(Gokai::ObjectArguments({}));
  auto binder = manager->getDefault();
  if (binder == nullptr) {
    auto all = manager->getAll();
    auto first = all.begin();
    if (first == all.end()) {
      spdlog::critical("Failed to find an available Gokai framework.");
      return EXIT_FAILURE;
    }
  }

  assert(binder != nullptr);

  auto factory = binder->getObjectFactory();
  auto logger = reinterpret_cast<Gokai::Logger*>(factory->createPointer(typeid(Gokai::Logger).name(), Gokai::ObjectArguments({})));

  auto context = static_cast<Gokai::Context*>(factory->createPointer(typeid(Gokai::Context).name(), Gokai::ObjectArguments({
    { "logger", logger },
  })));

  // TODO: use context to set up application

  delete context;
  delete manager;
  spdlog::shutdown();
  return EXIT_SUCCESS;
}