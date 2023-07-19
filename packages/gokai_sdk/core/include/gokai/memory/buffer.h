#pragma once

#ifdef __cplusplus
#include <gokai/object.h>
#include <memory>
#include <vector>

namespace Gokai {
  namespace Memory {
    class ReadBuffer;
    class WriteBuffer;
    class Buffer : public Gokai::Object {
      public:
        Buffer(Gokai::ObjectArguments arguments);

        virtual void seek(size_t pos);
        bool hasRemaining();
        size_t getPosition();
        size_t getSize();
        std::vector<uint8_t> toVector();

        ReadBuffer toReadBuffer();
        WriteBuffer toWriteBuffer();
      protected:
        size_t pos;
        std::vector<uint8_t> buffer;
    };
  }
}
#endif