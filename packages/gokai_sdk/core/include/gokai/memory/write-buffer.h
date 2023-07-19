#pragma once

#ifdef __cplusplus
#include <gokai/memory/buffer.h>
#include <memory>
#include <list>

namespace Gokai {
  namespace Memory {
    class WriteBuffer : public Buffer {
      public:
        WriteBuffer(Gokai::ObjectArguments arguments);

        void seek(size_t pos) override;

        void putUint8(uint8_t value);
        void putUint16(uint16_t value);
        void putUint32(uint32_t value);
        void putUint64(uint64_t value);

        void putUint8List(std::list<uint8_t> value);
        void putUint16List(std::list<uint16_t> value);
        void putUint32List(std::list<uint32_t> value);
        void putUint64List(std::list<uint64_t> value);
    };
  }
}
#endif