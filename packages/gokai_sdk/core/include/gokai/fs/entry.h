#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/object.h>
#include <chrono>
#include <filesystem>
#include <future>

namespace Gokai {
  namespace FS {
    class DirectoryEntry;
    class FileEntry;
    class LinkEntry;

    struct EntryStat {
      std::filesystem::path path;
      std::filesystem::file_type type;

      uint64_t dev;
      uint64_t mode;
      uint64_t nlink;
      uint64_t uid;
      uint64_t gid;
      uint64_t rdev;
      uint64_t ino;
      uint64_t size;
      uint64_t blksize;
      uint64_t blocks;
      uint64_t flags;
      uint64_t gen;

      std::chrono::time_point<std::chrono::system_clock> atime;
      std::chrono::time_point<std::chrono::system_clock> mtime;
      std::chrono::time_point<std::chrono::system_clock> ctime;
      std::chrono::time_point<std::chrono::system_clock> btime;

      EntryStat();
      EntryStat(uv_stat_t stat, std::filesystem::path path);
      EntryStat(Gokai::Context* context, std::filesystem::path path);

      static std::future<EntryStat> async(Gokai::Context* context, std::filesystem::path path);
    };

    class Entry : public Gokai::Object {
      public:
        Entry(Gokai::ObjectArguments arguments);

        std::filesystem::path getPath();

        virtual std::future<Entry> rename(std::filesystem::path path);
        virtual Entry renameSync(std::filesystem::path path);

        virtual std::future<bool> deleteAsync();
        virtual bool deleteSync();

        virtual std::future<bool> exists();
        virtual bool existsSync();

        virtual std::future<void> copy(std::filesystem::path path);
        virtual void copySync(std::filesystem::path path);

        virtual std::future<EntryStat> stat();
        virtual EntryStat statSync();

        virtual DirectoryEntry toDirectory();
        virtual FileEntry toFile();
        virtual LinkEntry toLink();
      protected:
        std::shared_ptr<Gokai::Context> context;
      private:
        std::filesystem::path path;
    };
  }
}
#endif
