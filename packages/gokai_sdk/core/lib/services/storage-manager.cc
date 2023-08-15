#include <gokai/services/storage-manager.h>

#define TAG "Gokai::Services::StorageManager"

using namespace Gokai::Services;

StorageManager::StorageManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    return std::async(std::launch::async, [this, engine_id, channel, message] {
      auto call = this->method_codec.decodeMethodCall(message);
      if (call.method.compare("stat") == 0) {
        auto path = std::filesystem::path(std::any_cast<std::string>(call.arguments));
        auto entry = this->open(path);

        auto stat = entry.statSync();
        std::map<std::string, std::any> map;
        map["path"] = stat.path.string();

        switch (stat.type) {
#define MAP_CASE(key) case std::filesystem::file_type::key: map["type"] = #key
          MAP_CASE(none);
          MAP_CASE(not_found);
          MAP_CASE(regular);
          MAP_CASE(directory);
          MAP_CASE(symlink);
          MAP_CASE(block);
          MAP_CASE(character);
          MAP_CASE(fifo);
          MAP_CASE(socket);
          MAP_CASE(unknown);
#undef MAP_CASE
        }

        map["dev"] = stat.dev;
        map["mode"] = stat.mode;
        map["nlink"] = stat.nlink;
        map["uid"] = stat.uid;
        map["gid"] = stat.gid;
        map["rdev"] = stat.rdev;
        map["ino"] = stat.ino;
        map["size"] = stat.size;
        map["blksize"] = stat.blksize;
        map["blocks"] = stat.blocks;
        map["flags"] = stat.flags;
        map["gen"] = stat.gen;

        map["atime"] = stat.atime.time_since_epoch();
        map["mtime"] = stat.mtime.time_since_epoch();
        map["ctime"] = stat.ctime.time_since_epoch();
        map["btime"] = stat.btime.time_since_epoch();
        return this->method_codec.encodeSuccessEnvelope(map);
      }

      if (call.method.compare("delete") == 0) {
        auto path = std::filesystem::path(std::any_cast<std::string>(call.arguments));
        auto entry = this->open(path);
        return this->method_codec.encodeSuccessEnvelope(entry.deleteSync());
      }

      if (call.method.compare("exists") == 0) {
        auto path = std::filesystem::path(std::any_cast<std::string>(call.arguments));
        auto entry = this->open(path);
        return this->method_codec.encodeSuccessEnvelope(entry.existsSync());
      }

      if (call.method.compare("copy") == 0) {
        auto args = std::any_cast<std::map<std::string, std::any>>(call.arguments);
        auto source = std::filesystem::path(std::any_cast<std::string>(args["source"]));
        auto dest = std::filesystem::path(std::any_cast<std::string>(args["dest"]));

        auto entry = this->open(source);
        entry.copySync(dest);
        return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
      }

      if (call.method.compare("rename") == 0) {
        auto args = std::any_cast<std::map<std::string, std::any>>(call.arguments);
        auto source = std::filesystem::path(std::any_cast<std::string>(args["source"]));
        auto dest = std::filesystem::path(std::any_cast<std::string>(args["dest"]));

        auto entry = this->open(source);
        entry.renameSync(dest);
        return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
      }
      return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
    });
  });
}

std::shared_ptr<Gokai::ServiceChannel> StorageManager::getServiceChannel() {
  return this->service_channel;
}

Gokai::FS::Entry StorageManager::open(std::filesystem::path path) {
  return Gokai::FS::Entry(Gokai::ObjectArguments({
    { "context", this->context },
    { "path", path },
  }));
}

const std::string StorageManager::SERVICE_NAME = "StorageManager";
