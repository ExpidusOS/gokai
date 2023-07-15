#pragma once

#ifdef __cplusplus
#include <assert.h>
#include <memory>
#include <thread>

namespace Gokai {
  namespace Graphics {
    template<class T> struct SurfaceBufferChain {
      SurfaceBufferChain() : thread_id{std::this_thread::get_id()} {}
      ~SurfaceBufferChain() {
        assert(thread_id == std::this_thread::get_id());
        render_buffer = nullptr;
        newest_buffer = nullptr;
      }

      [[nodiscard]] T* startRead() {
        assert(thread_id == std::this_thread::get_id());
        render_buffer = newest_buffer;
        return render_buffer.get();
      }

      void endRead() {
        assert(thread_id == std::this_thread::get_id());
        render_buffer = nullptr;
      }

      void commitBuffer(std::shared_ptr<T> buffer) {
        assert(thread_id == std::this_thread::get_id());
        newest_buffer = std::move(buffer);
      }

      std::shared_ptr<T> render_buffer = {};
      std::shared_ptr<T> newest_buffer = {};
      std::thread::id thread_id;
    };
  }
}
#endif