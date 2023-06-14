#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE(GokaiApplication, gokai_application, GOKAI, APPLICATION, GtkApplication);

struct _GokaiApplicationClass {
  GtkApplicationClass parent_class;
};

GokaiApplication* gokai_application_new(const char* application_id);

G_END_DECLS