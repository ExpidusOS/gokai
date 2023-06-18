#pragma once

#include <flutter_linux/flutter_linux.h>
#include <gokai/flutter/common.h>

G_BEGIN_DECLS

typedef struct _GokaiPlugin GokaiPlugin;
typedef struct {
  GObjectClass parent_class;
} GokaiPluginClass;

FLUTTER_PLUGIN_EXPORT GType gokai_plugin_get_type();
FLUTTER_PLUGIN_EXPORT void gokai_plugin_register_with_registrar(FlPluginRegistrar* registrar);

G_END_DECLS
