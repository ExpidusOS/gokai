#pragma once

#ifdef __cplusplus
#include <gokai/memory/buffer.h>
#include <memory>
#include <list>

namespace Gokai {
  namespace Memory {
    class ReadBuffer : public Buffer {
      public:
        ReadBuffer(Gokai::ObjectArguments arguments);

        void seek(size_t pos) override;

        uint8_t getUint8();
        uint16_t getUint16();
        uint32_t getUint32();
        uint64_t getUint64();

        std::list<uint8_t> getUint8List(size_t len);
        std::list<uint16_t> getUint16List(size_t len);
        std::list<uint32_t> getUint32List(size_t len);
        std::list<uint64_t> getUint64List(size_t len);
    };
  }
}
#endif