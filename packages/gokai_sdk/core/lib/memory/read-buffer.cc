#include <fmt/format.h>
#include <gokai/memory/read-buffer.h>
#include <stdexcept>

using namespace Gokai::Memory;

ReadBuffer::ReadBuffer(Gokai::ObjectArguments arguments) : Buffer(arguments) {}

void ReadBuffer::seek(size_t pos) {
  auto new_pos = this->pos + pos;
  if (new_pos < this->buffer.size()) {
    Buffer::seek(pos);
  } else {
    throw std::runtime_error(fmt::format("Cannot seek({}) due to it exceeding the buffer size ({})", pos, this->buffer.size()));
  }
}

uint8_t ReadBuffer::getUint8() {
  auto value = this->buffer[this->pos];
  this->seek(1);
  return value;
}

uint16_t ReadBuffer::getUint16() {
  return (this->getUint8() << 8) | this->getUint8();
}

uint32_t ReadBuffer::getUint32() {
  return (this->getUint8() << 24)
    | (this->getUint8() << 16)
    | (this->getUint8() << 8)
    | this->getUint8();
}

uint64_t ReadBuffer::getUint64() {
  return (this->getUint8() << 56)
    | (this->getUint8() << 48)
    | (this->getUint8() << 40)
    | (this->getUint8() << 32)
    | (this->getUint8() << 24)
    | (this->getUint8() << 16)
    | (this->getUint8() << 8)
    | this->getUint8();
}

std::list<uint8_t> ReadBuffer::getUint8List(size_t len) {
  std::list<uint8_t> list;
  while ((len--) > 0) list.push_back(this->getUint8());
  return list;
}

std::list<uint16_t> ReadBuffer::getUint16List(size_t len) {
  std::list<uint16_t> list;
  while ((len--) > 0) list.push_back(this->getUint16());
  return list;
}

std::list<uint32_t> ReadBuffer::getUint32List(size_t len) {
  std::list<uint32_t> list;
  while ((len--) > 0) list.push_back(this->getUint32());
  return list;
}

std::list<uint64_t> ReadBuffer::getUint64List(size_t len) {
  std::list<uint64_t> list;
  while ((len--) > 0) list.push_back(this->getUint32());
  return list;
}