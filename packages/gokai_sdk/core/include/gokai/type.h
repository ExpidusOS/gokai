#pragma once

#include <gokai/funcs.h>
#include <gokai/macros.h>
#include <stdint.h>

typedef uint64_t GokaiTypeId;
#define GOKAI_TYPE_NONE 0

typedef struct _GokaiType {
  GokaiTypeId id;
  const char* name;

  GokaiTypeId* extensions;
  size_t size;

  GokaiConstructFunc construct;
  GokaiDestroyFunc destroy;
  GokaiRefFunc ref;
  GokaiUnrefFunc unref;
} GokaiType;

typedef struct _GokaiTypeRegistry {
  GokaiType* (*get)(struct _GokaiTypeRegistry* self, GokaiTypeId id);
  GokaiTypeId (*lookup)(struct _GokaiTypeRegistry* self, const char* name);
  GokaiType* (*add)(struct _GokaiTypeRegistry* self, const char* name);
  void (*remove)(struct _GokaiTypeRegistry* self, GokaiType* type);
} GokaiTypeRegistry;

typedef struct _GokaiTypeInstance GokaiTypeInstance;

GOKAI_BEGIN_DECLS

GokaiTypeRegistry* gokai_type_registry_get_default();

GokaiType* gokai_type_registry_get(GokaiTypeRegistry* self, GokaiTypeId id);
GokaiTypeId gokai_type_registry_lookup(GokaiTypeRegistry* self, const char* name);
GokaiType* gokai_type_registry_add(GokaiTypeRegistry* self, const char* name);
void gokai_type_registry_remove(GokaiTypeRegistry* self, GokaiType* type);

GokaiTypeInstance* gokai_type_instance_cast(GokaiTypeInstance* self, GokaiTypeId id);

GOKAI_END_DECLS