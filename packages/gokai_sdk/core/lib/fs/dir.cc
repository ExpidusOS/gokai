#include <gokai/fs/dir.h>

using namespace Gokai::FS;

DirectoryEntry::DirectoryEntry(Gokai::ObjectArguments arguments) : Entry(arguments) {}

std::future<void> DirectoryEntry::link(std::filesystem::path path) {
  return std::async(std::launch::async, [this, path]() {
    this->linkSync(path);
  });
}

void DirectoryEntry::linkSync(std::filesystem::path path) {
  std::filesystem::create_directory_symlink(this->getPath(), path);
}

std::future<bool> DirectoryEntry::create(bool recursive) {
  return std::async(std::launch::async, [this, recursive]() {
    return this->createSync(recursive);
  });
}

bool DirectoryEntry::createSync(bool recursive) {
  return recursive ? std::filesystem::create_directories(this->getPath())
    : std::filesystem::create_directory(this->getPath());
}

std::future<std::list<Entry>> DirectoryEntry::list() {
  return std::async(std::launch::async, [this]() {
    return this->listSync();
  });
}

std::list<Entry> DirectoryEntry::listSync() {
  std::list<Entry> items;
  for (auto const& dir_entry : std::filesystem::directory_iterator{this->getPath()}) {
    items.push_back(Entry(Gokai::ObjectArguments({
      { "context", this->context },
      { "path", dir_entry.path() },
    })));
  }
  return items;
}
