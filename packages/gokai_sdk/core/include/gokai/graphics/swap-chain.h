#pragma once

#ifdef __cplusplus
#include <gokai/view/rect.h>
#include <memory>
#include <mutex>
#include <vector>

namespace Gokai {
  namespace Graphics {
    template<class T> struct Slot {
      explicit Slot(std::shared_ptr<T> buffer) : buffer{buffer} {}

      std::shared_ptr<T> buffer;
      std::vector<Gokai::View::DRect> damage_regions = {};
    };

    template<class T> struct SwapChain {
      explicit SwapChain(const std::array<std::shared_ptr<T>, 4>& buffers) {
        read_buffer1 = std::make_shared<Slot<T>>(buffers[0]);
        read_buffer2 = std::make_shared<Slot<T>>(buffers[1]);
        write_buffer = std::make_shared<Slot<T>>(buffers[2]);
        latest_buffer = std::make_shared<Slot<T>>(buffers[3]);
      }

      std::mutex mutex = {};

      std::shared_ptr<Slot<T>> read_buffer1 = {};
      std::shared_ptr<Slot<T>> read_buffer2 = {};
      std::shared_ptr<Slot<T>> write_buffer = {};
      std::shared_ptr<Slot<T>> latest_buffer = {};

      bool new_buffer_available = false;

      T* startWrite() {
        std::scoped_lock lock(mutex);
        return this->write_buffer->buffer.get();
      }

      T* startRead() {
        std::scoped_lock lock(mutex);
        if (new_buffer_available) {
          std::swap(latest_buffer, read_buffer1);
          std::swap(latest_buffer, read_buffer2);
          new_buffer_available = false;
        }
        return this->read_buffer1->buffer.get();
      }

      void endWrite(std::vector<Gokai::View::DRect> damage) {
        std::scoped_lock lock(mutex);
        write_buffer->damage_regions = damage;
        latest_buffer->damage_regions.insert(latest_buffer->damage_regions.end(), damage.begin(), damage.end());
        read_buffer1->damage_regions.insert(read_buffer1->damage_regions.end(), damage.begin(), damage.end());
      	read_buffer2->damage_regions.insert(read_buffer2->damage_regions.end(), damage.begin(), damage.end());

      	std::swap(write_buffer, latest_buffer);
      	new_buffer_available = true;
      }

      std::vector<Gokai::View::DRect> getDamageRegions() {
        std::scoped_lock lock(mutex);
        return write_buffer->damage_regions;
      }
    };
  }
}
#endif