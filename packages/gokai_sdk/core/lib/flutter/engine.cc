#include <gokai/flutter/engine.h>
#include <gokai/service.h>
#include <filesystem>
#include <stdexcept>
#include <unistd.h>

#define TAG "Gokai::Flutter::Engine"

using namespace Gokai::Flutter;

bool TaskRunnerComp::operator()(const EngineTask& a, const EngineTask& b) const {
  return a.first > b.first;
}

static void receive(const uint8_t* data, size_t size, void* user_data) {
  auto promise = reinterpret_cast<std::promise<std::vector<uint8_t>>*>(user_data);
  if (data == nullptr || size == 0) {
    promise->set_value(std::vector<uint8_t>());
    return;
  }

  promise->set_value(std::vector<uint8_t>(data, data + size));
}

bool Engine::runs_task_on_current_thread_callback(void* data) {
  Engine* self = reinterpret_cast<Engine*>(data);
  return self->thread_id == std::this_thread::get_id();
}

void Engine::post_task_callback(FlutterTask task, uint64_t target_time, void* data) {
  Engine* self = reinterpret_cast<Engine*>(data);
  if (self->shutdown) {
    self->logger->warn("Engine is in shutdown phase");
    return;
  }

  self->logger->debug("Queueing task {}", task.task);
  self->tasks.emplace(target_time, task);
}

void Engine::log_message_callback(const char* tag, const char* message, void* data) {
  auto self = reinterpret_cast<Engine*>(data);
  self->getLogger()->get(tag, Gokai::ObjectArguments({
    { "context", self->context.get() },
  }))->info("{}", message);
}

void Engine::platform_message_callback(const FlutterPlatformMessage* message, void* data) {
  auto self = reinterpret_cast<Engine*>(data);

  std::thread([self, message]() {
    self->logger->debug("Receiving message on method channel \"{}\" for engine {}", message->channel, self->id.str());

    std::vector<uint8_t> msg(message->message, message->message + message->message_size);

    auto find = self->method_channel_handlers.find(message->channel);
    if (find != self->method_channel_handlers.end()) {
      for (auto func : find->second) {
        auto msg_resp = func(msg);
        if (msg_resp.size() > 0) {
          auto result = FlutterEngineSendPlatformMessageResponse(
            self->value, message->response_handle, msg_resp.data(), msg_resp.size()
          );

          if (result != kSuccess) {
            self->logger->error("Failed to send response for engine {} on service channel \"{}\": {}", self->id.str(), message->channel, result);
          }
          return;
        }
      }
    }

    for (auto service_name : self->context->getSystemServiceNames()) {
      auto service = self->context->getSystemService(service_name);

      auto service_channel = service->getServiceChannel();
      if (service_channel == nullptr) continue;
      if (service_channel->getName().compare(message->channel) != 0) continue;

      auto msg_resp = service_channel->receive(self->id, msg);
      if (msg_resp.size() > 0) {
        auto result = FlutterEngineSendPlatformMessageResponse(
          self->value, message->response_handle, msg_resp.data(), msg_resp.size()
        );
        if (result != kSuccess) {
          self->logger->error("Failed to send response for engine {} on service channel \"{}\": {}", self->id.str(), message->channel, result);
        }
        return;
      }
    }

    if (strcmp(message->channel, "Gokai::Context") == 0) {
      auto msg_resp = self->context->channelReceive(self->id, msg);
      if (msg_resp.size() > 0) {
        auto result = FlutterEngineSendPlatformMessageResponse(
          self->value, message->response_handle, msg_resp.data(), msg_resp.size()
        );
        if (result != kSuccess) {
          self->logger->error("Failed to send response for engine {} on service channel \"{}\": {}", self->id.str(), message->channel, result);
        }
        return;
      }
    }

    self->logger->warn("A handler for the \"{}\" method channel on engine {} was not set, sending an empty response.", message->channel, self->id.str());

    std::vector<uint8_t> resp;
    auto result = FlutterEngineSendPlatformMessageResponse(
      self->value, message->response_handle, resp.data(), resp.size()
    );
    if (result != kSuccess) {
      self->logger->error("Failed to send response for engine {} on method channel \"{}\": {}", self->id.str(), message->channel, result);
    }
  }).detach();
}

Engine::Engine(Gokai::ObjectArguments arguments)
    : Gokai::Loggable(TAG, arguments),
      thread_id{std::this_thread::get_id()},
      shutdown{false},
      view_type{std::any_cast<EngineViewType>(arguments.get("view-type"))},
      view_name{std::any_cast<std::string>(arguments.get("view-name"))} {
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

  auto args = this->context->getArguments();
  auto argc = args.size();
  auto argv = reinterpret_cast<char**>(malloc(sizeof (char*) * (argc + 1)));
  memset(argv, 0, sizeof (char*) * (argc + 1));

  size_t i = 0;
  for (const auto& arg : args) {
    argv[i] = (char*)arg.c_str();
    i++;
  }

  this->args = {};
  this->args.command_line_argc = argc;
  this->args.command_line_argv = argv;

  this->args.struct_size = sizeof (FlutterProjectArgs);
  this->args.assets_path = strdup((path / "data" / "flutter_assets").c_str());
  this->args.icu_data_path = strdup((path / "data" / "icudtl.dat").c_str());
  this->args.log_tag = strdup(fmt::format("{}#{}", TAG, this->id.str()).c_str());
  this->args.log_message_callback = Engine::log_message_callback;
  this->args.platform_message_callback = Engine::platform_message_callback;
  // NOTE: https://github.com/flutter/flutter/issues/129533
  this->args.custom_task_runners = &this->runners;

  if (arguments.has("compositor")) {
    this->compositor = std::any_cast<Gokai::Graphics::Compositor*>(arguments.get("compositor"));
    this->args.compositor = this->compositor->get();
  } else {
    this->compositor = nullptr;
  }

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

  uv_timer_init(this->context->getLoop(), &this->event_runner);
  uv_timer_start(&this->event_runner, Engine::event_callback, 100, 100);

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
  this->shutdown = true;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  if (!this->tasks.empty()) {
    this->logger->warn("Waiting for engine {} to clear up tasks before shutting down", this->id.str());
    std::thread([this]() {
      while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (this->tasks.empty()) break;
      }

      uv_timer_stop(&this->event_runner);

      for (auto func : this->destroy) func();
      this->logger->debug("Shutting down engine {}", this->id.str());
      FlutterEngineShutdown(this->value);
    }).detach();
  } else {
    uv_timer_stop(&this->event_runner);

    for (auto func : this->destroy) func();
    this->logger->debug("Shutting down engine {}", this->id.str());
    FlutterEngineShutdown(this->value);
  }
}

bool Engine::isShutdown() {
  return this->shutdown;
}

std::future<std::vector<uint8_t>> Engine::send(std::string channel, std::vector<uint8_t> data) {
  auto promise = new std::promise<std::vector<uint8_t>>();

  FlutterPlatformMessageResponseHandle* handle = nullptr;
  auto result = FlutterPlatformMessageCreateResponseHandle(this->value, receive, promise, &handle);
  if (result != kSuccess) {
    throw std::runtime_error("Failed to create response handle");
  }

  FlutterPlatformMessage message = {};
  message.struct_size = sizeof (FlutterPlatformMessage);
  message.channel = channel.c_str();
  message.message = data.data();
  message.message_size = data.size();
  message.response_handle = handle;

  result = FlutterEngineSendPlatformMessage(this->value, &message);
  if (result != kSuccess) {
    throw std::runtime_error("Failed to send platform message");
  }
  return promise->get_future();
}

xg::Guid Engine::getId() {
  return this->id;
}

Gokai::Graphics::Compositor* Engine::getCompositor() {
  return this->compositor;
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

std::thread::id Engine::getThreadId() {
  return this->thread_id;
}

EngineViewType Engine::getViewType() {
  return this->view_type;
}

std::string Engine::getViewName() {
  return this->view_name;
}

void Engine::event_callback(uv_timer_t* event_runner) {
  Engine* self = reinterpret_cast<Engine*>((char*)(event_runner) - offsetof(Engine, event_runner));
  if (self->tasks.empty()) return;

  while (not self->tasks.empty() and FlutterEngineGetCurrentTime() >= self->tasks.top().first) {
    auto task = self->tasks.top();
    self->tasks.pop();
    self->logger->debug("Executing task {}", task.second.task);
    FlutterEngineRunTask(self->getValue(), &task.second);
  }
}
