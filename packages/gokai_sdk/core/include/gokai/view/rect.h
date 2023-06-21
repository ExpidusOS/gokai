#pragma once

#ifdef __cplusplus
#include <fmt/core.h>
#include <gokai/view/enums.h>
#include <glm/vec2.hpp>
#include <stdexcept>

namespace Gokai {
  namespace View {
    template<typename T> struct Rect {
      typedef glm::vec<2, T, glm::defaultp> Vector;
      Vector pos;
      Vector size;

      Rect(Vector pos, Vector size) : pos{pos}, size{size} {}
      Rect(Vector size) : Rect(Vector(), size) {}
      Rect() : Rect(Vector()) {}

      Vector side(Side value) {
        switch (value) {
          case Side::left:
          case Side::top:
            return this->pos;
          case Side::right:
            return Vector(this->pos.x + this->size.x, this->pos.y);
          case Side::bottom:
            return Vector(this->pos.x, this->pos.y + this->size.y);
          default:
            throw std::runtime_error("Unknown enum value");
        }
      }

      Vector corner(Corner value) {
        switch (value) {
          case Corner::top_left:
            return this->pos;
          case Corner::top_right:
            return Vector(this->pos.x + this->size.x, this->pos.y);
          case Corner::bottom_left:
            return Vector(this->pos.x, this->pos.y + this->size.y);
          case Corner::bottom_left:
            return this->pos + this->size;
          default:
            throw std::runtime_error("Unknown enum value");
        }
      }

      bool intersects(Rect<T> other) {
        return this->pos >= other->pos && (this->pos + this->size <= (other->pos + other->size));
      }

      Rect<T> operator-(Rect<T> other) {
        return Rect(
          this->pos - other.pos,
          this->size - other.size
        );
      }

      Rect<T> operator+(Rect<T> other) {
        return Rect(
          this->pos + other.pos,
          this->size + other.size
        );
      }

      Rect<T> operator*(Rect<T> other) {
        return Rect(
          this->pos * other.pos,
          this->size * other.size
        );
      }

      Rect<T> operator/(Rect<T> other) {
        return Rect(
          this->pos / other.pos,
          this->size / other.size
        );
      }

      Rect<T> operator*(T factor) {
        return Rect(
          this->pos,
          this->size * factor
        );
      }

      Rect<T> operator/(T factor) {
        return Rect(
          this->pos,
          this->size / factor
        );
      }

      bool operator<(Rect<T> other) {
        return this->size < other->size;
      }

      bool operator>(Rect<T> other) {
        return this->size > other->size;
      }

      bool operator==(Rect<T> other) {
        return this->size == other->size && this->pos == other->pos;
      }

      bool operator!=(Rect<T> other) {
        return this->size != other->size || this->pos != other->pos;
      }

      operator std::string() {
        return fmt::format(
          "Rect(x: {}, y: {}, w: {}, h: {})",
          this->pos.x,
          this->pos.y,
          this->size.x,
          this->size.y
        );
      }
    };

    typedef Rect<unsigned int> URect;
    typedef Rect<double> DRect;
  }
}
#endif