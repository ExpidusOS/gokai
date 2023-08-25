#include <gokai/fs/dir.h>
#include <gokai/fs/entry.h>
#include <gokai/fs/file.h>
#include <gokai/fs/link.h>
#include <stdexcept>
#include <uv.h>

using namespace Gokai::FS;

struct EntryStatEvent {
  uv_fs_t req;
  std::promise<uv_stat_t>* promise;

  EntryStatEvent(Gokai::Context* context, std::filesystem::path path) {
    this->promise = new std::promise<uv_stat_t>();

    uv_fs_stat(context->getLoop(), &this->req, path.c_str(), EntryStatEvent::callback);
  }

  static void callback(uv_fs_t* req) {
    auto self = reinterpret_cast<EntryStatEvent*>((char*)(req) - offsetof(EntryStatEvent, req));
    if (req->result >= 0) {
      self->promise->set_value(self->req.statbuf);
    } else {
      self->promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to stat.")));
    }
  }

  static uv_stat_t sync(Gokai::Context* context, std::filesystem::path path) {
    auto event = new EntryStatEvent(context, path);
    auto future = event->promise->get_future();
    future.wait();
    return future.get();
  }
};

EntryStat::EntryStat() {}

EntryStat::EntryStat(uv_stat_t stat, std::filesystem::path path) {
  this->path = path;
  this->type = std::filesystem::directory_entry(path).status().type();

  this->dev = stat.st_dev;
  this->mode = stat.st_mode;
  this->nlink = stat.st_nlink;
  this->uid = stat.st_uid;
  this->gid = stat.st_gid;
  this->rdev = stat.st_rdev;
  this->ino = stat.st_ino;
  this->size = stat.st_size;
  this->blksize = stat.st_blksize;
  this->blocks = stat.st_blocks;
  this->flags = stat.st_flags;
  this->gen = stat.st_gen;

#define TIME_CONV(spec) std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::seconds((spec).tv_sec) + std::chrono::nanoseconds((spec).tv_nsec))}
  this->atime = TIME_CONV(stat.st_atim);
  this->mtime = TIME_CONV(stat.st_mtim);
  this->ctime = TIME_CONV(stat.st_ctim);
  this->btime = TIME_CONV(stat.st_birthtim);
#undef TIME_CONV
}

EntryStat::EntryStat(Gokai::Context* context, std::filesystem::path path) : EntryStat(EntryStatEvent::sync(context, path), path) {}

std::future<EntryStat> EntryStat::async(Gokai::Context* context, std::filesystem::path path) {
  auto event = new EntryStatEvent(context, path);
  return std::async(std::launch::async, [event, path]() {
    auto future = event->promise->get_future();
    future.wait();
    return EntryStat(future.get(), path);
  });
}

Entry::Entry(Gokai::ObjectArguments arguments) : Gokai::Object(arguments) {
  this->context = std::any_cast<std::shared_ptr<Gokai::Context>>(arguments.get("context"));
  this->path = std::any_cast<std::filesystem::path>(arguments.get("path"));
}

std::filesystem::path Entry::getPath() {
  return this->path;
}

std::shared_ptr<Gokai::Context> Entry::getContext() {
  return this->context;
}

std::future<Entry> Entry::rename(std::filesystem::path path) {
  return std::async(std::launch::async, [this, path]() {
    return this->renameSync(path);
  });
}

Entry Entry::renameSync(std::filesystem::path path) {
  std::filesystem::rename(this->getPath(), path);
  return Entry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", path },
  }));
}

std::future<bool> Entry::exists() {
  return std::async(std::launch::async, [this]() {
    return this->existsSync();
  });
}

std::future<bool> Entry::deleteAsync() {
  return std::async(std::launch::async, [this]() {
    return this->deleteSync();
  });
}

bool Entry::deleteSync() {
  return std::filesystem::remove_all(this->getPath());
}

bool Entry::existsSync() {
  return std::filesystem::exists(this->getPath());
}

std::future<void> Entry::copy(std::filesystem::path path) {
  return std::async(std::launch::async, [this, path]() {
    this->copySync(path);
  });
}

void Entry::copySync(std::filesystem::path path) {
  std::filesystem::copy(this->getPath(), path);
}

std::future<EntryStat> Entry::stat() {
  return EntryStat::async(this->context.get(), this->getPath());
}

EntryStat Entry::statSync() {
  return EntryStat(this->context.get(), this->getPath());
}

DirectoryEntry Entry::toDirectory() {
  return DirectoryEntry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", this->path },
  }));
}

FileEntry Entry::toFile() {
  return FileEntry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", this->path },
  }));
}

LinkEntry Entry::toLink() {
  return LinkEntry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", this->path },
  }));
}
