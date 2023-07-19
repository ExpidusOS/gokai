#include <gokai/memory/buffer.h>
#include <gokai/memory/read-buffer.h>
#include <gokai/memory/write-buffer.h>

using namespace Gokai::Memory;

Buffer::Buffer(Gokai::ObjectArguments arguments) : Object(arguments), pos{0} {
  if (arguments.has("buffer")) {
    this->buffer = std::any_cast<std::vector<uint8_t>>(arguments.get("buffer"));
  }

  if (arguments.has("position")) {
    this->pos = std::any_cast<size_t>(arguments.get("position"));
  }
}

void Buffer::seek(size_t pos) {
  this->pos += pos;
}

bool Buffer::hasRemaining() {
  return this->pos < this->buffer.size();
}

size_t Buffer::getPosition() {
  return this->pos;
}

size_t Buffer::getSize() {
  return this->buffer.size();
}

std::vector<uint8_t> Buffer::toVector() {
  return this->buffer;
}

ReadBuffer Buffer::toReadBuffer() {
  return ReadBuffer(Gokai::ObjectArguments({
    { "buffer", this->buffer },
    { "position", this->pos },
  }));
}

WriteBuffer Buffer::toWriteBuffer() {
  return WriteBuffer(Gokai::ObjectArguments({
    { "buffer", this->buffer },
    { "position", this->pos },
  }));
}