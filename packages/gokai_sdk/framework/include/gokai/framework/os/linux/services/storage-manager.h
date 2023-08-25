#pragma once

#ifdef __cplusplus
#include <gokai/services/storage-manager.h>

namespace Gokai {
  namespace Framework {
    namespace os {
      namespace Linux {
        namespace Services {
          class StorageManager : public Gokai::Services::StorageManager {
            public:
              StorageManager(Gokai::ObjectArguments arguments);

              Gokai::FS::Entry open(std::filesystem::path path) override;
          };
        }
      }
    }
  }
}
#endif
