#pragma once

#ifdef __cplusplus
#include <gokai/fs/file.h>
#include <gokai/framework/os/linux/fs/entry.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace FS {
          class FileEntry : public Entry, public Gokai::FS::FileEntry {
            public:
              FileEntry(Gokai::ObjectArguments arguments);

              bool createSync(bool recursive, bool exclusive) override;
              void linkSync(std::filesystem::path path) override;
              std::vector<char> readSync() override;
              void writeSync(std::vector<char> buffer) override;
          };
        }
      }
    }
  }
}
#endif
