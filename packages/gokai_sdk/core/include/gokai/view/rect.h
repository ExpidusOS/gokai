#pragma once

#ifdef __cplusplus
#include <gokai/view/enums.h>
#include <glm/vec2.hpp>

namespace Gokai {
  namespace View {
    template<typename T> struct Rect {
      glm::vec<2, T, glm::defaultp> pos;
      glm::vec<2, T, glm::defaultp> size;

      Rect(glm::vec<2, T, glm::defaultp> pos, glm::vec<2, T, glm::defaultp> size);
      Rect(glm::vec<2, T, glm::defaultp> size);

      glm::vec<2, T, glm::defaultp> side(Side side);
      glm::vec<2, T, glm::defaultp> corner(Corner corner);

      bool intersects(Rect<T> other);

      Rect<T> operator-(Rect<T> other);
      Rect<T> operator+(Rect<T> other);
      Rect<T> operator*(Rect<T> other);
      Rect<T> operator/(Rect<T> other);

      Rect<T> operator*(T factor);
      Rect<T> operator/(T factor);

      bool operator<(Rect<T> other);
      bool operator>(Rect<T> other);

      bool operator==(Rect<T> other);
      bool operator!=(Rect<T> other);
    };

    typedef Rect<unsigned int> URect;
    typedef Rect<double> DRect;
  }
}
#endif