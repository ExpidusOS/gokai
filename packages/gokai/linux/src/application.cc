#include <gokai/flutter/application.h>
#include <any>
#include <gokai/api/os/linux/binder-manager.h>
#include <gokai/api/binder.h>
#include <gokai/context.h>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

typedef struct _GokaiApplicationPrivate {
  Gokai::API::BinderManager* binder_mngr;
  Gokai::API::Binder* binder;
  Gokai::ObjectFactory* obj_factory;
  Gokai::Context* context;
} GokaiApplicationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(GokaiApplication, gokai_application, GTK_TYPE_APPLICATION);

static void gokai_application_constructed(GObject* obj) {
  G_OBJECT_CLASS(gokai_application_parent_class)->constructed(obj);

  GokaiApplication* self = GOKAI_APPLICATION(obj);
  GokaiApplicationPrivate* priv = reinterpret_cast<GokaiApplicationPrivate*>(gokai_application_get_instance_private(self));

  priv->binder_mngr = new Gokai::API::os::Linux::BinderManager(Gokai::ObjectArguments({}));
  priv->binder = priv->binder_mngr->getDefault();

  if (priv->binder == nullptr) {
    auto all = priv->binder_mngr->getAll();
    auto first = all.begin();
    if (first == all.end()) g_critical("Failed to bind to Gokai.");
    priv->binder = first->second;
  }

  priv->obj_factory = priv->binder->getObjectFactory();
  spdlog::info("Loaded Gokai framework: {}", priv->binder->getPath());

  // TODO: get the arguments from the command line
  try {
    priv->context = reinterpret_cast<Gokai::Context*>(priv->obj_factory->createPointer(typeid(Gokai::Context).name(), Gokai::ObjectArguments({
      { "mode", std::string("compositor") },
      { "display-backend", std::string("wayland") },
    })));
  } catch (const std::exception& ex) {
    spdlog::critical("Failed to create a context: {}", ex.what());
    abort();
  }

  spdlog::debug("Created the Gokai::Context: {}", reinterpret_cast<void*>(priv->context));
}

static void gokai_application_dispose(GObject* obj) {
  GokaiApplication* self = GOKAI_APPLICATION(obj);
  GokaiApplicationPrivate* priv = reinterpret_cast<GokaiApplicationPrivate*>(gokai_application_get_instance_private(self));

  delete priv->context;
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
  spdlog::cfg::load_env_levels();
  return GOKAI_APPLICATION(g_object_new(gokai_application_get_type(),
    "application-id", application_id,
    "flags", G_APPLICATION_NON_UNIQUE,
    nullptr
  ));
}