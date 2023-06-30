#pragma once

#ifdef __cplusplus
#include <algorithm>

namespace Gokai {
  namespace Flutter {
    class TextRange {
      public:
        explicit TextRange(size_t position);
        explicit TextRange(size_t base, size_t extent);

        TextRange(const TextRange&) = default;
        TextRange& operator=(const TextRange&) = default;
        virtual ~TextRange() = default;

        size_t getBase() const;
        void setBase(size_t pos);

        size_t getExtent() const;
        void setExtent(size_t pos);

        void setStart(size_t pos);
        size_t getStart() const;

        void setEnd(size_t pos);
        size_t getEnd() const;

        size_t getPosition();
        size_t getLength();

        bool isCollapsed() const;
        bool isReversed() const;
        bool contains(size_t pos) const;
        bool contains(const TextRange& range) const;
        bool operator==(const TextRange& other) const;
      private:
        size_t base;
        size_t extent;
    };
  }
}
#endif