#pragma once

#ifdef __cplusplus
#include <gokai/os/dynamic-library.h>
#include <gokai/context.h>
#include <gokai/logging.h>
#include <gokai/object.h>

namespace Gokai {
  struct PluginMetadata {
    const char* name;
    const char* description;
    const char* homepage;
    const char* license;
  };

  class Plugin : public Gokai::Loggable {
    public:
      Plugin(Gokai::ObjectArguments arguments);
    protected:
      std::shared_ptr<Context> context;
  };

  typedef Plugin* (*PluginLoadFunc)(Gokai::ObjectArguments arguments);
}
#endif
