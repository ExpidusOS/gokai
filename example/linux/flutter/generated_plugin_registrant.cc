//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <gokai/gokai_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) gokai_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "GokaiPlugin");
  gokai_plugin_register_with_registrar(gokai_registrar);
}
