#pragma once

#include <gokai/funcs.h>
#include <gokai/macros.h>
#include <stddef.h>

typedef struct _GokaiList {
  struct _GokaiList* prev;
  void* data;
  GokaiDestroyFunc destroy;
  struct _GokaiList* next;
} GokaiList;

#define gokai_list_foreach_head(list, item) gokai_list_foreach(list, item, gokai_list_get_head)
#define gokai_list_foreach_tail(list, item) gokai_list_foreach(list, item, gokai_list_get_tail)
#define gokai_list_foreach(list, item, strategy) for (GokaiList* item = strategy(list); item != NULL; item = item->next)

GOKAI_BEGIN_DECLS

GokaiList* gokai_list_get_head(GokaiList* list);
GokaiList* gokai_list_get_tail(GokaiList* list);
size_t gokai_list_length(GokaiList* list);

GokaiList* gokai_list_alloc(void* data, GokaiDestroyFunc destroy);
GokaiList* gokai_list_append(GokaiList* list, void* data, GokaiDestroyFunc destroy);
GokaiList* gokai_list_prepend(GokaiList* list, void* data, GokaiDestroyFunc destroy);
GokaiList* gokai_list_remove(GokaiList* list);

GOKAI_END_DECLS