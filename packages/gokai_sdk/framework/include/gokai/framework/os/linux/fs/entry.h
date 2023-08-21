#pragma once

#ifdef __cplusplus
#include <gokai/fs/entry.h>
#include <gio/gio.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace FS {
          class Entry : public Gokai::FS::Entry {
            public:
              Entry(Gokai::ObjectArguments arguments);
              ~Entry();

              GFile* getValue();
            private:
              GFile* value;
          };
        }
      }
    }
  }
}
#endif
