#include <gokai/api/os/linux/binder-manager.h>
#include <gokai/flutter/common.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/cfg/env.h>
#include <assert.h>

extern "C" FLUTTER_PLUGIN_EXPORT int main(int argc, char** argv) {
  auto out_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  out_sink->set_level(spdlog::level::trace);

  auto err_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
  err_sink->set_level(spdlog::level::err);

  auto default_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger(argv[0], { out_sink, err_sink }));
  spdlog::register_logger(default_logger);
  spdlog::set_default_logger(default_logger);

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

  std::list<std::string> args;
  for (int i = 0; i < argc; i++) {
    args.push_back(strdup(argv[i]));
  }

  try {
    auto context = static_cast<Gokai::Context*>(factory->createPointer(typeid(Gokai::Context).name(), Gokai::ObjectArguments({
      { "logger", logger },
      { "arguments", args },
    })));

    // TODO: use context to set up application

    uv_run(context->getLoop(), UV_RUN_DEFAULT);
    delete context;
  } catch (const std::exception& ex) {
    spdlog::critical("Caught exception: {}", ex.what());
    delete manager;
    spdlog::shutdown();
    return EXIT_FAILURE;
  }

  delete manager;
  spdlog::shutdown();
  return EXIT_SUCCESS;
}
