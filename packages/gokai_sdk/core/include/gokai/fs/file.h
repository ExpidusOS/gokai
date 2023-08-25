#pragma once

#ifdef __cplusplus
#include <gokai/fs/entry.h>

namespace Gokai {
  namespace FS {
    class FileEntry : public Gokai::FS::Entry {
      public:
        FileEntry(Gokai::ObjectArguments arguments);
        FileEntry(Gokai::FS::Entry& src);

        virtual std::future<bool> create(bool recursive, bool exclusive);
        virtual bool createSync(bool recursive, bool exclusive);

        virtual std::future<void> link(std::filesystem::path path);
        virtual void linkSync(std::filesystem::path path);

        virtual std::future<std::vector<char>> read();
        virtual std::vector<char> readSync();

        virtual std::future<void> write(std::vector<char> buffer);
        virtual void writeSync(std::vector<char> buffer);
    };
  }
}
#endif
