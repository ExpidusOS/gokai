#include <gokai/memory/write-buffer.h>

using namespace Gokai::Memory;

WriteBuffer::WriteBuffer(Gokai::ObjectArguments arguments) : Buffer(arguments) {}

void WriteBuffer::seek(size_t pos) {
  for (size_t i = 0; i < pos; i++) this->buffer.push_back(0);
  Buffer::seek(pos);
}

void WriteBuffer::putUint8(uint8_t value) {
  this->buffer.push_back(value);
  Buffer::seek(1);
}

void WriteBuffer::putUint16(uint16_t value) {
  this->buffer.push_back((value >> 8) & 0xFF);
  this->buffer.push_back(value & 0xFF);
  Buffer::seek(2);
}

void WriteBuffer::putUint32(uint32_t value) {
  this->buffer.push_back((value >> 24) & 0xFF);
  this->buffer.push_back((value >> 16) & 0xFF);
  this->buffer.push_back((value >> 8) & 0xFF);
  this->buffer.push_back(value & 0xFF);
  Buffer::seek(4);
}

void WriteBuffer::putUint64(uint64_t value) {
  this->buffer.push_back((value >> 56) & 0xFF);
  this->buffer.push_back((value >> 48) & 0xFF);
  this->buffer.push_back((value >> 40) & 0xFF);
  this->buffer.push_back((value >> 32) & 0xFF);
  this->buffer.push_back((value >> 24) & 0xFF);
  this->buffer.push_back((value >> 16) & 0xFF);
  this->buffer.push_back((value >> 8) & 0xFF);
  this->buffer.push_back(value & 0xFF);
  Buffer::seek(8);
}

void WriteBuffer::putUint8List(std::list<uint8_t> value) {
  for (const auto& item : value) this->putUint8(item);
}

void WriteBuffer::putUint16List(std::list<uint16_t> value) {
  for (const auto& item : value) this->putUint16(item);
}

void WriteBuffer::putUint32List(std::list<uint32_t> value) {
  for (const auto& item : value) this->putUint32(item);
}

void WriteBuffer::putUint64List(std::list<uint64_t> value) {
  for (const auto& item : value) this->putUint64(item);
}