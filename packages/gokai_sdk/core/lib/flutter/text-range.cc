#include <gokai/flutter/text-range.h>

using namespace Gokai::Flutter;

TextRange::TextRange(size_t position) : base(position), extent(position) {}
TextRange::TextRange(size_t base, size_t extent) : base(base), extent(extent) {}

size_t TextRange::getBase() const {
  return this->base;
}

void TextRange::setBase(size_t pos) {
  this->base = pos;
}

size_t TextRange::getExtent() const {
  return this->extent;
}

void TextRange::setExtent(size_t extent) {
  this->extent = extent;
}

void TextRange::setStart(size_t pos) {
  if (this->base <= this->extent) {
    this->base = pos;
  } else {
    this->extent = pos;
  }
}

size_t TextRange::getStart() const {
  return std::min(this->base, this->extent);
}

void TextRange::setEnd(size_t pos) {
  if (this->base <= this->extent) {
    this->extent = pos;
  } else {
    this->base = pos;
  }
}

size_t TextRange::getEnd() const {
  return std::max(this->base, this->extent);
}

size_t TextRange::getPosition() {
  return this->extent;
}

size_t TextRange::getLength() {
  return this->getEnd() - this->getStart();
}

bool TextRange::isCollapsed() const {
  return this->base == this->extent;
}

bool TextRange::isReversed() const {
  return this->base > this->extent;
}

bool TextRange::contains(size_t pos) const {
  return pos >= this->getStart() && pos <= this->getEnd();
}

bool TextRange::contains(const TextRange& range) const {
  return range.getStart() >= this->getStart() && range.getEnd() <= this->getEnd();
}

bool TextRange::operator==(const TextRange& other) const {
  return this->base == other.base && this->extent == other.extent;
}