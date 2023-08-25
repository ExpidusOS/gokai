#pragma once

#ifdef __cplusplus
#include <gokai/fs/entry.h>

namespace Gokai {
  namespace FS {
    class DirectoryEntry : public Entry {
      public:
        DirectoryEntry(Gokai::ObjectArguments arguments);
        DirectoryEntry(Gokai::FS::Entry& src);

        virtual std::future<void> link(std::filesystem::path path);
        virtual void linkSync(std::filesystem::path path);

        virtual std::future<bool> create(bool recursive);
        virtual bool createSync(bool recursive);

        virtual std::future<std::list<Entry>> list();
        virtual std::list<Entry> listSync();
    };
  }
}
#endif
