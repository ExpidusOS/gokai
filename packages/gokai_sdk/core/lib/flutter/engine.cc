#include <gokai/flutter/engine.h>
#include <gokai/service.h>
#include <filesystem>
#include <stdexcept>
#include <unistd.h>

#define TAG "Gokai::Flutter::Engine"

using namespace Gokai::Flutter;

void EngineTask::callback(uv_timer_t* handle) {
  EngineTask* self = reinterpret_cast<EngineTask*>((char*)(handle) - offsetof(EngineTask, handle));

  self->engine->logger->debug("Running task {}", reinterpret_cast<void*>(&self->task));
  auto result = FlutterEngineRunTask(self->engine->getValue(), &self->task);
  if (result != kSuccess) {
    self->engine->logger->error("Failed to run task: {}", result);
  }

  uv_timer_stop(&self->handle);
  delete self;
}

bool Engine::runs_task_on_current_thread_callback(void* data) {
  Engine* self = reinterpret_cast<Engine*>(data);
  return self->pid == uv_os_getpid();
}

void Engine::post_task_callback(FlutterTask task, uint64_t target_time, void* data) {
  Engine* self = reinterpret_cast<Engine*>(data);

  auto etask = new EngineTask();
  etask->engine = self;
  etask->task = task;
  etask->handle = {};

  self->logger->debug("Queueing task {}", reinterpret_cast<void*>(&etask->task));

  uv_timer_init(self->context->getLoop(), &etask->handle);
  uv_timer_start(&etask->handle, EngineTask::callback, 0, 0);
}

void Engine::log_message_callback(const char* tag, const char* message, void* data) {
  auto self = reinterpret_cast<Engine*>(data);
  self->getLogger()->get(tag, Gokai::ObjectArguments({
    { "context", self->context.get() },
  }))->info("{}", message);
}

void Engine::platform_message_callback(const FlutterPlatformMessage* message, void* data) {
  auto self = reinterpret_cast<Engine*>(data);

  self->logger->debug("Receiving message on method channel \"{}\" for engine {}", message->channel, self->id.str());

  for (auto service_name : self->context->getSystemServiceNames()) {
    auto service = self->context->getSystemService(service_name);

    auto service_channel = service->getServiceChannel();
    if (service_channel == nullptr) continue;
    if (service_channel->getName().compare(message->channel) != 0) continue;

    std::vector<uint8_t> msg(message->message, message->message + message->message_size);
    auto msg_resp = service_channel->receive(self->id, msg);

    auto result = FlutterEngineSendPlatformMessageResponse(
      self->value, message->response_handle, msg_resp.data(), msg_resp.size()
    );
    if (result != kSuccess) {
      self->logger->error("Failed to send response for engine {} on service channel \"{}\": {}", self->id.str(), message->channel, result);
    }
    return;
  }

  self->logger->warn("A handler for the \"{}\" method channel on engine {} was not set, sending an empty response.", message->channel, self->id.str());

  std::vector<uint8_t> resp;
  auto result = FlutterEngineSendPlatformMessageResponse(
    self->value, message->response_handle, resp.data(), resp.size()
  );
  if (result != kSuccess) {
    self->logger->error("Failed to send response for engine {} on method channel \"{}\": {}", self->id.str(), message->channel, result);
  }
}

Engine::Engine(Gokai::ObjectArguments arguments) : Gokai::Loggable(TAG, arguments), pid{uv_os_getpid()} {
  if (arguments.has("id")) {
    this->id = std::any_cast<xg::Guid>(arguments.get("id"));
  } else {
    this->id = xg::newGuid();
  }

  this->renderer = std::any_cast<Gokai::Graphics::Renderer*>(arguments.get("renderer"));
  this->context = std::any_cast<std::shared_ptr<Gokai::Context>>(arguments.get("context"));

  auto path = std::filesystem::path(this->context->getPackageDir());

  this->runner_task = {
    .struct_size = sizeof (FlutterTaskRunnerDescription),
    .user_data = reinterpret_cast<void*>(this),
    .runs_task_on_current_thread_callback = Engine::runs_task_on_current_thread_callback,
    .post_task_callback = Engine::post_task_callback,
  };

  this->runners = {
    .struct_size = sizeof (FlutterCustomTaskRunners),
    .platform_task_runner = &this->runner_task,
    .render_task_runner = &this->runner_task,
    .thread_priority_setter = nullptr,
  };

  this->args = {};
  this->args.struct_size = sizeof (FlutterProjectArgs);
  this->args.assets_path = strdup((path / "data" / "flutter_assets").c_str());
  this->args.icu_data_path = strdup((path / "data" / "icudtl.dat").c_str());
  this->args.log_tag = strdup(fmt::format("{}#{}", TAG, this->id.str()).c_str());
  this->args.log_message_callback = Engine::log_message_callback;
  this->args.platform_message_callback = Engine::platform_message_callback;
  // NOTE: https://github.com/flutter/flutter/issues/129533
  this->args.custom_task_runners = &this->runners;

  if (FlutterEngineRunsAOTCompiledDartCode()) {
    FlutterEngineAOTDataSource src = {
      .type = kFlutterEngineAOTDataSourceTypeElfPath,
      .elf_path = strdup((path / "lib" / "libapp.so").c_str()),
    };

    auto result = FlutterEngineCreateAOTData(&src, &this->args.aot_data);
    if (result != kSuccess) {
      throw std::runtime_error(fmt::format("Failed to create AOT data: {}", result));
    }
  }

  auto result = FlutterEngineInitialize(
    FLUTTER_ENGINE_VERSION,
    this->renderer->getConfig(),
    &this->args,
    this,
    &this->value
  );
  if (result != kSuccess) {
    throw std::runtime_error(fmt::format("Failed to initialize the engine: {}", result));
  }

  this->logger->debug("Flutter engine {} has initialized", this->id.str());

  if ((result = FlutterEngineRunInitialized(this->value)) != kSuccess) {
    throw std::runtime_error(fmt::format("Failed to start the engine: {}", result));
  }

  auto size = this->renderer->getSize();

  FlutterWindowMetricsEvent metrics = {
    .struct_size = sizeof (FlutterWindowMetricsEvent),
    .width = size.x,
    .height = size.y,
    .pixel_ratio = 1.0,
  };

  this->logger->debug("Initial window metrics (w: {}, h: {}, r: {}) for engine {}", metrics.width, metrics.height, metrics.pixel_ratio, this->id.str());

  if (FlutterEngineSendWindowMetricsEvent(this->value, &metrics) != kSuccess) {
    throw std::runtime_error(fmt::format(
      "Failed to initialize window metrics ({}, {}) for engine {}",
      metrics.width,
      metrics.height,
      this->id.str()
    ));
  }
}

Engine::~Engine() {
  this->logger->debug("Shutting down engine {}", this->id.str());
  FlutterEngineShutdown(this->value);
}

xg::Guid Engine::getId() {
  return this->id;
}

Gokai::Graphics::Renderer* Engine::getRenderer() {
  return this->renderer;
}

std::shared_ptr<Gokai::Context> Engine::getContext() {
  return this->context;
}

FlutterEngine Engine::getValue() {
  return this->value;
}

void Engine::resize(glm::uvec2 size) {
  this->renderer->resize(size);

  FlutterWindowMetricsEvent metrics = {
    .struct_size = sizeof (FlutterWindowMetricsEvent),
    .width = size.x,
    .height = size.y,
    .pixel_ratio = 1.0,
  };

  this->logger->debug("Updating window metrics (w: {}, h: {}, r: {}) {} for engine {}", metrics.width, metrics.height, metrics.pixel_ratio, reinterpret_cast<void*>(this->value), this->id.str());

  auto result = FlutterEngineSendWindowMetricsEvent(this->value, &metrics);
  if (result != kSuccess) {
    throw std::runtime_error(fmt::format(
      "Failed to update window metrics ({}, {}) for engine {}",
      metrics.width,
      metrics.height,
      this->id.str()
    ));
  }
}

uv_pid_t Engine::getPid() {
  return this->pid;
}