#include <gokai/framework/os/linux/logging.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/cfg/env.h>

using namespace Gokai::Framework::os::Linux;

Logger::Logger(Gokai::ObjectArguments arguments) : Gokai::Logger(arguments) {
  this->logger = this->get("Gokai::Logger");
}

std::shared_ptr<spdlog::logger> Logger::get(const char* tag) {
  auto out_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  out_sink->set_level(spdlog::level::trace);

  auto err_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
  err_sink->set_level(spdlog::level::err);

  auto logger = std::shared_ptr<spdlog::logger>(new spdlog::logger(tag, { out_sink, err_sink }));
  spdlog::register_logger(logger);
  spdlog::cfg::load_env_levels();

  if (this->logger != nullptr) {
    this->logger->debug("Created logger {}", logger->name());
  }
  return logger;
}