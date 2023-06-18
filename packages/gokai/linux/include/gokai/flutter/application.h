#pragma once

#include <gokai/flutter/common.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE(GokaiApplication, gokai_application, GOKAI, APPLICATION, GtkApplication);

struct _GokaiApplicationClass {
  GtkApplicationClass parent_class;
};

FLUTTER_PLUGIN_EXPORT GokaiApplication* gokai_application_new(const char* application_id);

G_END_DECLS