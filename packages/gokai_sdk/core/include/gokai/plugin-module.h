#pragma once

#ifdef __cplusplus
#include <gokai/os/dynamic-library.h>
#include <gokai/context.h>
#include <gokai/object.h>
#include <gokai/plugin.h>

namespace Gokai {
  class PluginModule : public Gokai::Object {
    public:
      PluginModule(Gokai::ObjectArguments arguments);
      ~PluginModule();

      PluginMetadata getMetadata();
      Gokai::Plugin* getImpl();
    private:
      PluginMetadata metadata;
      Gokai::os::DynamicLibrary* lib;
      Gokai::Plugin* impl;
  };
}
#endif
