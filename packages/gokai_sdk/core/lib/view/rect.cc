#include <gokai/view/rect.h>
#include <stdexcept>

using namespace Gokai::View;

template<typename T> Rect<T>::Rect(glm::vec<2, T, glm::defaultp> pos, glm::vec<2, T, glm::defaultp> size) : pos{pos}, size{size} {}

template<typename T> Rect<T>::Rect(glm::vec<2, T, glm::defaultp> size) : Rect(glm::vec<2, T, glm::defaultp>(0, 0), size) {}

template<typename T> glm::vec<2, T, glm::defaultp> Rect<T>::side(Side side) {
  switch (side) {
    case Side::left:
    case Side::top:
      return this->pos;
    case Side::right:
      return glm::vec<2, T, glm::defaultp>(this->pos.x + this->size.x, this->pos.y);
    case Side::bottom:
      return glm::vec<2, T, glm::defaultp>(this->pos.x, this->pos.y + this->size.y);
    default:
      throw std::runtime_error("Unknown enum value");
  }
}

template<typename T> glm::vec<2, T, glm::defaultp> Rect<T>::corner(Corner corner) {
  switch (corner) {
    case Corner::top_left:
      return this->pos;
    case Corner::top_right:
      return glm::vec<2, T, glm::defaultp>(this->pos.x + this->size.x, this->pos.y);
    case Corner::bottom_left:
      return glm::vec<2, T, glm::defaultp>(this->pos.x, this->pos.y + this->size.y);
    case Corner::bottom_left:
      return this->pos + this->size;
    default:
      throw std::runtime_error("Unknown enum value");
  }
}

template<typename T> bool Rect<T>::intersects(Rect<T> other) {
  return this->pos >= other->pos && (this->pos + this->size <= (other->pos + other->size));
}

template<typename T> Rect<T> Rect<T>::operator-(Rect<T> other) {
  return Rect<T>(
    this->pos - other.pos,
    this->size - other.size
  );
}

template<typename T> Rect<T> Rect<T>::operator+(Rect<T> other) {
  return Rect<T>(
    this->pos + other.pos,
    this->size + other.size
  );
}

template<typename T> Rect<T> Rect<T>::operator*(Rect<T> other) {
  return Rect<T>(
    this->pos * other.pos,
    this->size * other.size
  );
}

template<typename T> Rect<T> Rect<T>::operator/(Rect<T> other) {
  return Rect<T>(
    this->pos / other.pos,
    this->size / other.size
  );
}

template<typename T> Rect<T> Rect<T>::operator*(T factor) {
  return Rect<T>(
    this->pos,
    this->size * factor
  );
}

template<typename T> Rect<T> Rect<T>::operator/(T factor) {
  return Rect<T>(
    this->pos,
    this->size / factor
  );
}

template<typename T> bool Rect<T>::operator<(Rect<T> other) {
  return this->size < other->size;
}

template<typename T> bool Rect<T>::operator>(Rect<T> other) {
  return this->size > other->size;
}

template<typename T> bool Rect<T>::operator==(Rect<T> other) {
  return this->size == other->size && this->pos == other->pos;
}

template<typename T> bool Rect<T>::operator!=(Rect<T> other) {
  return this->size != other->size || this->pos != other->pos;
}