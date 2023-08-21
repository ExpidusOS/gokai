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

              Gokai::FS::Entry renameSync(std::filesystem::path path) override;
              bool deleteSync() override;
              bool existsSync() override;
              void copySync(std::filesystem::path path) override;
              Gokai::FS::EntryStat statSync() override;
            private:
              GVfs* fs;
              GFile* value;
          };
        }
      }
    }
  }
}
#endif
