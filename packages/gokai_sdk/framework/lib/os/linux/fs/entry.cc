#include <fmt/core.h>
#include <gokai/framework/os/linux/fs/dir.h>
#include <gokai/framework/os/linux/fs/entry.h>
#include <stdexcept>

using namespace Gokai::Framework::os::Linux::FS;

static std::chrono::time_point<std::chrono::system_clock> convert_datetime(GDateTime* dt) {
  if (dt == nullptr) {
    return std::chrono::system_clock::time_point::min();
  }

  auto epoch = std::chrono::seconds(g_date_time_to_unix(dt));
  return std::chrono::system_clock::time_point(epoch);
}

Entry::Entry(Gokai::ObjectArguments arguments) : Gokai::FS::Entry(arguments), fs{nullptr} {
  if (arguments.has("vfs")) {
    this->fs = std::any_cast<GVfs*>(arguments.get("vfs"));
  } else {
    this->fs = g_object_ref(g_vfs_get_default());
  }

  if (this->fs == nullptr) {
    throw std::runtime_error("VFS was not properly given");
  }

  this->value = g_vfs_get_file_for_path(this->fs, this->getPath().c_str());
  if (this->value == nullptr) {
    throw std::runtime_error("Could not get a file");
  }
}

Entry::~Entry() {
  g_clear_object(&this->value);
  g_clear_object(&this->fs);
}

GFile* Entry::getValue() {
  return this->value;
}

Gokai::FS::Entry Entry::renameSync(std::filesystem::path path) {
  auto dest = g_vfs_get_file_for_path(this->fs, path.c_str());
  if (dest == nullptr) {
    throw std::runtime_error("Could not get a file");
  }

  GError* error = nullptr;
  if (!g_file_move(this->getValue(), dest, G_FILE_COPY_NONE, nullptr, nullptr, nullptr, &error)) {
    g_object_unref(dest);
    throw std::runtime_error(fmt::format(
      "Could not move {} to {}: {}:{}: {}",
      this->getPath().string(),
      path.string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  g_object_unref(dest);
  return Entry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", path },
    { "vfs", g_object_ref(this->fs) },
  }));
}

bool Entry::deleteSync() {
  GError* error = nullptr;
  if (!g_file_delete(this->getValue(), nullptr, &error)) {
    throw std::runtime_error(fmt::format(
      "Could not delete {}: {}:{}: {}",
      this->getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }
  return true;
}

bool Entry::existsSync() {
  return g_file_query_exists(this->getValue(), nullptr);
}

void Entry::copySync(std::filesystem::path path) {
  auto dest = g_vfs_get_file_for_path(this->fs, path.c_str());
  if (dest == nullptr) {
    throw std::runtime_error("Could not get a file");
  }

  GError* error = nullptr;
  if (!g_file_copy(this->getValue(), dest, G_FILE_COPY_NONE, nullptr, nullptr, nullptr, &error)) {
    g_object_unref(dest);
    throw std::runtime_error(fmt::format(
      "Could not move {} to {}: {}:{}: {}",
      this->getPath().string(),
      path.string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  g_object_unref(dest);
}

Gokai::FS::EntryStat Entry::statSync() {
  GError* error = nullptr;
  auto info = g_file_query_info(this->getValue(), "*", G_FILE_QUERY_INFO_NONE, nullptr, &error);
  if (info == nullptr) {
    throw std::runtime_error(fmt::format(
      "Could not get info for {}: {}:{}: {}",
      this->getPath().string(),
      g_quark_to_string(error->domain),
      error->code,
      error->message
    ));
  }

  Gokai::FS::EntryStat stat;
  stat.path = this->getPath();

  switch (g_file_info_get_file_type(info)) {
    case G_FILE_TYPE_UNKNOWN:
      stat.type = std::filesystem::file_type::unknown;
      break;
    case G_FILE_TYPE_REGULAR:
      stat.type = std::filesystem::file_type::regular;
      break;
    case G_FILE_TYPE_DIRECTORY:
      stat.type = std::filesystem::file_type::directory;
      break;
    case G_FILE_TYPE_SYMBOLIC_LINK:
      stat.type = std::filesystem::file_type::symlink;
      break;
    default:
      stat.type = std::filesystem::file_type::none;
      break;
  }

#define SET_ATTRIB(up, lo, type) \
    if (g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_ ## up)) { \
      stat.lo = g_file_info_get_attribute_ ## type (info, G_FILE_ATTRIBUTE_ ## up); \
    }

  SET_ATTRIB(UNIX_DEVICE, dev, uint32);
  SET_ATTRIB(UNIX_MODE, mode, uint32);
  SET_ATTRIB(UNIX_NLINK, nlink, uint32);
  SET_ATTRIB(UNIX_UID, uid, uint32);
  SET_ATTRIB(UNIX_GID, gid, uint32);
  SET_ATTRIB(UNIX_RDEV, rdev, uint32);
  SET_ATTRIB(UNIX_INODE, ino, uint64);
  SET_ATTRIB(UNIX_BLOCK_SIZE, blksize, uint32);
  SET_ATTRIB(UNIX_BLOCKS, blocks, uint64);

#undef SET_ATTRIB

  stat.size = g_file_info_get_size(info);
  stat.atime = convert_datetime(g_file_info_get_access_date_time(info));
  stat.mtime = convert_datetime(g_file_info_get_modification_date_time(info));
  stat.ctime = std::chrono::system_clock::time_point::min();
  stat.btime = convert_datetime(g_file_info_get_creation_date_time(info));

  g_object_unref(info);
  return stat;
}

Gokai::FS::DirectoryEntry Entry::toDirectory() {
  return DirectoryEntry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", this->getPath() },
    { "vfs", g_object_ref(this->fs) },
  }));
}
