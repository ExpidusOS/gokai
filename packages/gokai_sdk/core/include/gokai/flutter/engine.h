#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/graphics/compositor.h>
#include <gokai/graphics/renderer.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <atomic>
#include <flutter_embedder.h>
#include <future>
#include <mutex>
#include <thread>
#include <queue>

namespace Gokai {
  namespace Flutter {
    using EngineTask = std::pair<uint64_t, FlutterTask>;

    struct TaskRunnerComp {
      bool operator()(const EngineTask& a, const EngineTask& b) const;
    };

    enum EngineViewType {
      window,
      display,
    };

    class Engine : public Gokai::Loggable {
      public:
        Engine(Gokai::ObjectArguments arguments);
        ~Engine();

        std::map<std::string, std::list<std::function<std::vector<uint8_t>(std::vector<uint8_t>)>>> method_channel_handlers;
        std::list<std::function<void()>> destroy;

        std::future<std::vector<uint8_t>> send(std::string channel, std::vector<uint8_t> data);
        xg::Guid getId();
        Gokai::Graphics::Compositor* getCompositor();
        Gokai::Graphics::Renderer* getRenderer();
        std::shared_ptr<Gokai::Context> getContext();
        FlutterEngine getValue();
        void resize(glm::uvec2 size);
        std::thread::id getThreadId();
        EngineViewType getViewType();
        std::string getViewName();
      private:
        bool shutdown;
        xg::Guid id;
        FlutterProjectArgs args;
        FlutterEngine value;
        FlutterCustomTaskRunners runners;
        FlutterTaskRunnerDescription runner_task;
        std::thread::id thread_id;
        uv_timer_t event_runner;
        std::priority_queue<EngineTask, std::vector<EngineTask>, TaskRunnerComp> tasks;
        EngineViewType view_type;
        std::string view_name;

        Gokai::Graphics::Compositor* compositor;
        Gokai::Graphics::Renderer* renderer;
        std::shared_ptr<Gokai::Context> context;

        static bool runs_task_on_current_thread_callback(void* data);
        static void post_task_callback(FlutterTask task, uint64_t target_time, void* data);
        static void log_message_callback(const char* tag, const char* message, void* data);
        static void platform_message_callback(const FlutterPlatformMessage* message, void* data);
        static void event_callback(uv_timer_t* event_runner);
    };
  }
}
#endif