#include <gokai/list.h>
#include <gokai/type.h>
#include <assert.h>
#include "type-priv.h"

GokaiType* gokai_type_registry_get(GokaiTypeRegistry* self, GokaiTypeId id) {
  if (id == GOKAI_TYPE_NONE) return NULL;

  assert(self != NULL);
  assert(self->get != NULL);
  return self->get(self, id);
}

GokaiTypeId gokai_type_registry_lookup(GokaiTypeRegistry* self, const char* name) {
  if (name == NULL) return GOKAI_TYPE_NONE;

  assert(self != NULL);
  assert(self->lookup != NULL);
  return self->lookup(self, name);
}

GokaiType* gokai_type_registry_add(GokaiTypeRegistry* self, const char* name) {
  if (name == GOKAI_TYPE_NONE) return NULL;

  assert(self != NULL);
  if (gokai_type_registry_lookup(self, name) > GOKAI_TYPE_NONE) return NULL;
  if (self->add == NULL) return NULL;
  return self->add(self, name);
}

void gokai_type_registry_remove(GokaiTypeRegistry* self, GokaiType* type) {
  assert(self != NULL);
  assert(type != NULL);
  if (self->remove != NULL) self->remove(self, type);
}

size_t gokai_type_registry_get_total_size(GokaiTypeRegistry* self, GokaiTypeId id) {
  GokaiType* type = gokai_type_registry_get(self, id);
  if (type == NULL) return 0;

  size_t curr_size = sizeof (GokaiTypeInstance) + type->size;
  for (size_t i = 0; type->extensions[i] != GOKAI_TYPE_NONE; i++) {
    GokaiType* parent = gokai_type_registry_get(self, type->extensions[i]);
    if (parent == NULL) continue;
    curr_size += gokai_type_registry_get_total_size(self, parent->id);
  }
  return curr_size;
}

GokaiTypeInstance* gokai_type_instance_cast(GokaiTypeInstance* self, GokaiTypeId id) {
  GokaiType* type = gokai_type_registry_get(self->registry, self->id);
  if (type == NULL) return NULL;
  if (self->id == id) return self;

  size_t curr_offset = sizeof (GokaiTypeInstance);
  for (size_t i = 0; type->extensions[i] != GOKAI_TYPE_NONE; i++) {
    GokaiType* parent = gokai_type_registry_get(self->registry, type->extensions[i]);
    if (parent == NULL) continue;

    GokaiTypeInstance* value = (GokaiTypeInstance*)((char*)(self) + curr_offset);
    if (parent->id == id) return value;

    GokaiTypeInstance* lookup = gokai_type_instance_cast(value, id);
    if (lookup != NULL) return lookup;

    curr_offset += gokai_type_registry_get_total_size(self->registry, parent->id);
  }
  return NULL;
}