#include <gokai/framework/os/linux/fs/dir.h>

using namespace Gokai::Framework::os::Linux::FS;

DirectoryEntry::DirectoryEntry(Gokai::ObjectArguments arguments) : Gokai::Framework::os::Linux::FS::Entry(arguments), Gokai::FS::DirectoryEntry(arguments) {
}

void DirectoryEntry::linkSync(std::filesystem::path path) {
  auto f = g_vfs_get_file_for_path(this->fs, path.c_str());
  if (f == nullptr) {
    throw std::runtime_error("Could not get a file");
  }

  GError* error = nullptr;
  if (!g_file_make_symbolic_link(f, this->Gokai::FS::DirectoryEntry::getPath().c_str(), nullptr, &error)) {
    g_object_unref(f);
    throw std::runtime_error(fmt::format(
      "Could not link {} to {}: {}:{}: {}",
      path.string(),
      this->Gokai::FS::DirectoryEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  g_object_unref(f);
}

bool DirectoryEntry::createSync(bool recursive) {
  GError* error = nullptr;
  if (!(recursive ? g_file_make_directory_with_parents(this->getValue(), nullptr, &error)
      : g_file_make_directory(this->getValue(), nullptr, &error))) {
    throw std::runtime_error(fmt::format(
      "Could not create {}: {}:{}: {}",
      this->Gokai::FS::DirectoryEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  return true;
}

std::list<Gokai::FS::Entry> DirectoryEntry::listSync() {
  GError* error = nullptr;
  auto e = g_file_enumerate_children(this->getValue(), "standard::", G_FILE_QUERY_INFO_NONE, nullptr, &error);
  if (e == nullptr) {
    throw std::runtime_error(fmt::format(
      "Could not enumerate {}: {}:{}: {}",
      this->Gokai::FS::DirectoryEntry::getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  std::list<Gokai::FS::Entry> list;
  while (true) {
    GFile* f = nullptr;
    if (!g_file_enumerator_iterate(e, nullptr, &f, nullptr, &error)) {
      throw std::runtime_error(fmt::format(
        "Could not iterate in {}: {}:{}: {}",
        this->Gokai::FS::DirectoryEntry::getPath().string(),
        g_quark_to_string(error->domain),
        error->code,
        error->message
      ));
    }

    list.push_back(Gokai::Framework::os::Linux::FS::Entry(Gokai::ObjectArguments({
      { "context", this->Gokai::FS::DirectoryEntry::context },
      { "path", std::filesystem::path(g_file_get_path(f)) },
      { "vfs", g_object_ref(this->fs) },
    })));

    g_object_unref(f);
  }

  g_object_unref(e);
  return list;
}
