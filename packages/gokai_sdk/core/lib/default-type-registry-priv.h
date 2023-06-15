#pragma once

#include <gokai/list.h>
#include <gokai/type.h>

typedef struct _GokaiDefaultTypeRegistry {
  GokaiList* types;
  GokaiTypeRegistry registry;
} GokaiDefaultTypeRegistry;

#define GOKAI_DEFAULT_TYPE_REGISTRY(registry) ((GokaiDefaultTypeRegistry*)((char*)(registry) - offsetof(GokaiDefaultTypeRegistry, registry)))