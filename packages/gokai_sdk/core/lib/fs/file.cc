#include <fmt/core.h>
#include <gokai/fs/dir.h>
#include <gokai/fs/file.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace Gokai::FS;

FileEntry::FileEntry(Gokai::ObjectArguments arguments) : Entry(arguments) {}

FileEntry::FileEntry(Gokai::FS::Entry& src) : Entry(Gokai::ObjectArguments({
  { "context", src.getContext() },
  { "path", src.getPath() },
})) {}

std::future<bool> FileEntry::create(bool recursive, bool exclusive) {
  return std::async(std::launch::async, [this, recursive, exclusive]() {
    return this->createSync(recursive, exclusive);
  });
}

bool FileEntry::createSync(bool recursive, bool exclusive) {
  auto dir = DirectoryEntry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", this->getPath().parent_path() },
  }));

  if (!dir.existsSync()) {
    if (!dir.createSync(recursive)) return false;
  }

  if (this->existsSync()) {
    if (!exclusive) {
      throw std::runtime_error(fmt::format("File {} already exists", this->getPath().string()));
    }
    return true;
  }

  std::fstream stream;
  stream.open(this->getPath(), std::ios::out);
  stream.close();
  return true;
}

std::future<void> FileEntry::link(std::filesystem::path path) {
  return std::async(std::launch::async, [this, path]() {
    this->linkSync(path);
  });
}

void FileEntry::linkSync(std::filesystem::path path) {
  std::filesystem::create_symlink(this->getPath(), path);
}

std::future<std::vector<char>> FileEntry::read() {
  return std::async(std::launch::async, [this]() {
    return this->readSync();
  });
}

std::vector<char> FileEntry::readSync() {
  std::ifstream stream(this->getPath(), std::ifstream::binary);
  if (!stream.is_open()) {
    throw std::runtime_error(fmt::format("Failed to open {}", this->getPath().string()));
  }

  auto size = std::filesystem::file_size(this->getPath());
  auto buffer = new char[size];

  stream.read(buffer, size);

  if (stream) {
    return std::vector(buffer, buffer + size);
  }

  throw std::runtime_error(fmt::format("Failed to read all of {}, {} bytes remaining", this->getPath().string(), size - stream.gcount()));
}

std::future<void> FileEntry::write(std::vector<char> buffer) {
  return std::async(std::launch::async, [this, buffer]() {
    this->writeSync(buffer);
  });
}

void FileEntry::writeSync(std::vector<char> buffer) {
  std::ofstream stream(this->getPath());
  if (!stream.is_open()) {
    throw std::runtime_error(fmt::format("Failed to open {}", this->getPath().string()));
  }

  stream.write(buffer.data(), buffer.size());
}
