#include <gokai/os/paths.h>
#include <sys/stat.h>
#include <assert.h>
#include <filesystem>
#include <limits.h>
#include <string.h>
#include <stdexcept>
#include <stdlib.h>
#include <unistd.h>

std::string Gokai::os::Paths::getRuntimePath() {
  struct stat sb;
  lstat("/proc/self/exe", &sb);

  size_t linksize = sb.st_size + 1;
  if (linksize == 1) linksize = PATH_MAX;

  char* link_path = reinterpret_cast<char*>(malloc(linksize));
  assert(link_path != nullptr);

  size_t read_size = readlink("/proc/self/exe", link_path, linksize);
  if (read_size < 0) {
    throw std::runtime_error("Failed to readlink /proc/self/exe");
  }

  link_path[read_size] = 0;

  auto str = strdup(std::filesystem::path(link_path).parent_path().c_str());
  free(link_path);
  return std::string(str);
}
