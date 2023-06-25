#include <gokai/flutter/engine.h>
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

  auto delta = target_time - FlutterEngineGetCurrentTime();
  self->logger->debug("Running task {} in {}", reinterpret_cast<void*>(&etask->task), delta);

  uv_timer_init(self->context->getLoop(), &etask->handle);
  uv_timer_start(&etask->handle, EngineTask::callback, delta, 0);
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
  // FIXME: [FATAL:flutter/fml/memory/weak_ptr.h(109)] Check failed: (checker_.checker).IsCreationThreadCurrent().
  // this->args.custom_task_runners = &this->runners;

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