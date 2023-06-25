#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <gokai/graphics/renderer.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <flutter_embedder.h>

namespace Gokai {
  namespace Flutter {
    class Engine : public Gokai::Loggable {
      public:
        Engine(Gokai::ObjectArguments arguments);
        ~Engine();

        xg::Guid getId();
        Gokai::Graphics::Renderer* getRenderer();
        std::shared_ptr<Gokai::Context> getContext();
        FlutterEngine getValue();
        void resize(glm::uvec2 size);
      private:
        xg::Guid id;
        FlutterProjectArgs args;
        FlutterEngine value;
        FlutterCustomTaskRunners runners;
        FlutterTaskRunnerDescription runner_task;
        uv_pid_t pid;

        Gokai::Graphics::Renderer* renderer;
        std::shared_ptr<Gokai::Context> context;

        static bool runs_task_on_current_thread_callback(void* data);
        static void post_task_callback(FlutterTask task, uint64_t target_time, void* data);
    };

    struct EngineTask {
      Engine* engine;
      FlutterTask task;
      uv_timer_t handle;

      static void callback(uv_timer_t* handle);
    };
  }
}
#endif