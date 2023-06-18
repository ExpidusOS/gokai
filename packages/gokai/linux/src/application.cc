#include <gokai/flutter/application.h>
#include <gokai/api/binder.h>
#include <gokai/api/binder-manager.h>

typedef struct _GokaiApplicationPrivate {
  Gokai::API::BinderManager* binder_mngr;
} GokaiApplicationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GokaiApplication, gokai_application, GTK_TYPE_APPLICATION);

static void gokai_application_constructed(GObject* obj) {
  G_OBJECT_CLASS(gokai_application_parent_class)->constructed(obj);

  GokaiApplication* self = GOKAI_APPLICATION(obj);
  GokaiApplicationPrivate* priv = reinterpret_cast<GokaiApplicationPrivate*>(gokai_application_get_instance_private(self));

  priv->binder_mngr = Gokai::API::BinderManager::create(Gokai::ObjectArguments({}));
}

static void gokai_application_dispose(GObject* obj) {
  GokaiApplication* self = GOKAI_APPLICATION(obj);
  GokaiApplicationPrivate* priv = reinterpret_cast<GokaiApplicationPrivate*>(gokai_application_get_instance_private(self));

  delete priv->binder_mngr;

  G_OBJECT_CLASS(gokai_application_parent_class)->dispose(obj);
}

static void gokai_application_class_init(GokaiApplicationClass* klass) {
  GObjectClass* obj_class = G_OBJECT_CLASS(klass);

  obj_class->constructed = gokai_application_constructed;
  obj_class->dispose = gokai_application_dispose;
}

static void gokai_application_init(GokaiApplication* self) {}

GokaiApplication* gokai_application_new(const char* application_id) {
  return GOKAI_APPLICATION(g_object_new(gokai_application_get_type(),
    "application-id", application_id,
    "flags", G_APPLICATION_NON_UNIQUE,
    nullptr
  ));
}