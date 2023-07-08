#pragma once

#ifdef __cplusplus
#include <crossguid/guid.hpp>
#include <any>
#include <stdint.h>

namespace Gokai {
  namespace User {
    enum IDType {
      guid,
      uid
    };

    struct ID {
      IDType type;

      union {
        xg::Guid guid;
        uint32_t uid;
      } data;

      ID(xg::Guid guid);
      ID(uint32_t uid);
      ID(std::any value);

      std::any toAny();
    };
  }
}
#endif
