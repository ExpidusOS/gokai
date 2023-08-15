#pragma once

#ifdef __cplusplus
#include <gokai/fs/entry.h>

namespace Gokai {
  namespace FS {
    class LinkEntry : public Gokai::FS::Entry {
      public:
        LinkEntry(Gokai::ObjectArguments arguments);
    };
  }
}
#endif
