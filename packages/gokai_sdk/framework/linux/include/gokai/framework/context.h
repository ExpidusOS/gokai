#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <string>

namespace Gokai {
  namespace Framework {
    class Context : public Gokai::Context {
    public:
      Service* getSystemService(std::string serviceName) override;
      std::string getPackageName() override;
      std::string getPackageDir() override;
    };
  }
}
#endif