#include <gokai/framework/os/linux/logging.h>
#include <gokai/context.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/syslog_sink.h>
#include <spdlog/sinks/systemd_sink.h>
#include <spdlog/cfg/env.h>
#include <assert.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux;

Logger::Logger(Gokai::ObjectArguments arguments) : Gokai::Logger(arguments) {
  this->logger = this->get("Gokai::Logger", arguments);
}

std::shared_ptr<spdlog::logger> Logger::get(const char* tag, std::string type) {
  if (type.compare("syslog") == 0) {
    return spdlog::syslog_logger_mt(tag, tag);
  } else if (type.compare("systemd") == 0) {
    return spdlog::systemd_logger_mt(tag, tag);
  } else if (type.compare("default") == 0) {
    auto out_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    out_sink->set_level(spdlog::level::trace);

    auto err_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    err_sink->set_level(spdlog::level::err);

    auto value = std::shared_ptr<spdlog::logger>(new spdlog::logger(tag, { out_sink, err_sink }));
    spdlog::register_logger(value);
    return value;
  }

  throw std::runtime_error("Unsupported logger type: " + type);
}

std::shared_ptr<spdlog::logger> Logger::get(const char* tag, Gokai::ObjectArguments arguments) {
  if (spdlog::get(tag) != nullptr) return spdlog::get(tag);

  std::shared_ptr<spdlog::logger> value = nullptr;
  std::string type = "default";
  if (arguments.has("context")) {
    auto context = std::any_cast<Gokai::Context*>(arguments.get("context"));
    auto manifest = context->getManifest();
    auto entry = manifest.defaults.find("Gokai::Framework::os::Linux::Logger");
    if (entry != manifest.defaults.end()) type = entry->second;

    auto env = getenv("GOKAI_LOGGER");
    if (env != nullptr) type = env;

    entry = manifest.overrides.find("Gokai::Framework::os::Linux::Logger");
    if (entry != manifest.overrides.end()) type = entry->second;
  } else {
    auto env = getenv("GOKAI_LOGGER");
    if (env != nullptr) type = env;
  }

  value = this->get(tag, type);
  assert(value != nullptr);
  spdlog::cfg::load_env_levels();

  if (this->logger != nullptr) {
    this->logger->debug("Created logger {}", value->name());
  }
  return value;
}