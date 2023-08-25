#include <fmt/core.h>
#include <gokai/framework/os/linux/fs/dir.h>
#include <gokai/framework/os/linux/fs/file.h>

using namespace Gokai::Framework::os::Linux::FS;

FileEntry::FileEntry(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::FS::Entry(arguments), Gokai::FS::FileEntry(arguments) {}

bool FileEntry::createSync(bool recursive, bool exclusive) {
  auto dir = DirectoryEntry(Gokai::ObjectArguments({
    { "context", this->Gokai::FS::FileEntry::context },
    { "path", this->Gokai::FS::FileEntry::getPath().parent_path() },
  }));

  if (!dir.Gokai::FS::DirectoryEntry::existsSync()) {
    if (!dir.createSync(recursive)) return false;
  }

  if (this->Gokai::FS::FileEntry::existsSync()) {
    if (!exclusive) {
      throw std::runtime_error(fmt::format("File {} already exists", this->Gokai::FS::FileEntry::getPath().string()));
    }
    return true;
  }

  GError* error = nullptr;
  auto stream = g_file_create(this->getValue(), G_FILE_CREATE_NONE, nullptr, &error);
  if (stream = nullptr) {
    throw std::runtime_error(fmt::format(
      "Could not create {}: {}:{}: {}",
      this->Gokai::FS::FileEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  g_object_unref(stream);
  return true;
}

void FileEntry::linkSync(std::filesystem::path path) {
  auto f = g_vfs_get_file_for_path(this->fs, path.c_str());
  if (f == nullptr) {
    throw std::runtime_error("Could not get a file");
  }

  GError* error = nullptr;
  if (!g_file_make_symbolic_link(f, this->Gokai::FS::FileEntry::getPath().c_str(), nullptr, &error)) {
    g_object_unref(f);
    throw std::runtime_error(fmt::format(
      "Could not link {} to {}: {}:{}: {}",
      path.string(),
      this->Gokai::FS::FileEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  g_object_unref(f);
}

std::vector<char> FileEntry::readSync() {
  GError* error = nullptr;
  gchar* contents = nullptr;
  gsize length = 0;
  if (!g_file_load_contents(this->getValue(), nullptr, &contents, &length, nullptr, &error)) {
    throw std::runtime_error(fmt::format(
      "Could not read {}: {}:{}: {}",
      this->Gokai::FS::FileEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  return std::vector<char>(contents, contents + length);
}

void FileEntry::writeSync(std::vector<char> buffer) {
  GError* error = nullptr;
  if (!g_file_replace_contents(this->getValue(), buffer.data(), buffer.size(), nullptr, false, G_FILE_CREATE_NONE, nullptr, nullptr, &error)) {
    throw std::runtime_error(fmt::format(
      "Could not write to {}: {}:{}: {}",
      this->Gokai::FS::FileEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }
}
