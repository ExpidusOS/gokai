#include <gokai/list.h>
#include <gokai/type.h>
#include "default-type-registry-priv.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static GokaiType* gokai_default_type_registry_get(GokaiTypeRegistry* self, GokaiTypeId id);
static GokaiTypeId gokai_default_type_registry_lookup(GokaiTypeRegistry* self, const char* name);
static GokaiType* gokai_default_type_registry_add(GokaiTypeRegistry* self, const char* name);
static void gokai_default_type_registry_remove(GokaiTypeRegistry* self, GokaiType* type);

static GokaiDefaultTypeRegistry gokai_default_type_registry = {
  .types = NULL,
  .registry = {
    .get = gokai_default_type_registry_get,
    .lookup = gokai_default_type_registry_lookup,
    .add = gokai_default_type_registry_add,
    .remove = gokai_default_type_registry_remove,
  },
};

GokaiTypeRegistry* gokai_type_registry_get_default() {
  return &gokai_default_type_registry.registry;
}

static GokaiType* gokai_default_type_registry_get(GokaiTypeRegistry* registry, GokaiTypeId id) {
  GokaiDefaultTypeRegistry* self = GOKAI_DEFAULT_TYPE_REGISTRY(registry);
  assert(self != NULL);

  gokai_list_foreach_head(self->types, item) {
    GokaiType* type = item->data;
    if (type->id == id) return type;
  }
  return NULL;
}

static GokaiTypeId gokai_default_type_registry_lookup(GokaiTypeRegistry* registry, const char* name) {
  GokaiDefaultTypeRegistry* self = GOKAI_DEFAULT_TYPE_REGISTRY(registry);
  assert(self != NULL);

  gokai_list_foreach_head(self->types, item) {
    GokaiType* type = item->data;
    if (strcmp(type->name, name) == 0) return type->id;
  }
  return GOKAI_TYPE_NONE;
}

static GokaiType* gokai_default_type_registry_add(GokaiTypeRegistry* registry, const char* name) {
  GokaiDefaultTypeRegistry* self = GOKAI_DEFAULT_TYPE_REGISTRY(registry);
  assert(self != NULL);

  GokaiType* type = malloc(sizeof (GokaiType));
  assert(type != NULL);
  memset(type, 0, sizeof (GokaiType));

  type->id = gokai_list_length(self->types) + 1;
  type->name = name;

  self->types = gokai_list_append(self->types, type, free);
  return type;
}

static void gokai_default_type_registry_remove(GokaiTypeRegistry* registry, GokaiType* type) {
  GokaiDefaultTypeRegistry* self = GOKAI_DEFAULT_TYPE_REGISTRY(registry);
  assert(self != NULL);

  gokai_list_foreach_head(self->types, item) {
    GokaiType* entry = item->data;
    if (entry->id == type->id && strcmp(entry->name, type->name) == 0) {
      self->types = gokai_list_remove(item);
      break;
    }
  }
}