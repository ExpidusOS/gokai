#pragma once

#ifdef __cplusplus
#include <gokai/fs/dir.h>
#include <gokai/framework/os/linux/fs/entry.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace FS {
          class DirectoryEntry : public Entry, public Gokai::FS::DirectoryEntry {
            public:
              DirectoryEntry(Gokai::ObjectArguments arguments);

              void linkSync(std::filesystem::path path) override;
              bool createSync(bool recursive) override;
              std::list<Gokai::FS::Entry> listSync() override;
          };
        }
      }
    }
  }
}
#endif
