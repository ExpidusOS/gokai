#include <gokai/list.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

GokaiList* gokai_list_get_head(GokaiList* list) {
  GokaiList* item = list;
  while (item != NULL) {
    if (item->prev == NULL) return item;
    item = item->prev;
  }
  return NULL;
}

GokaiList* gokai_list_get_tail(GokaiList* list) {
  GokaiList* item = list;
  while (item != NULL) {
    if (item->next == NULL) return item;
    item = item->next;
  }
  return NULL;
}

size_t gokai_list_length(GokaiList* list) {
  size_t count = 0;
  gokai_list_foreach_head(list, item) count++;
  return count;
}

GokaiList* gokai_list_alloc(void* data, GokaiDestroyFunc destroy) {
  GokaiList* item = malloc(sizeof (GokaiList));
  assert(item != NULL);
  memset(item, 0, sizeof (GokaiList));

  item->data = data;
  item->destroy = destroy;
  return item;
}

GokaiList* gokai_list_append(GokaiList* list, void* data, GokaiDestroyFunc destroy) {
  if (list == NULL) {
    return gokai_list_alloc(data, destroy);
  }

  GokaiList* tail = gokai_list_get_tail(list);
  assert(tail != NULL);

  GokaiList* item = gokai_list_alloc(data, destroy);

  item->prev = tail;
  tail->next = item;

  return list;
}

GokaiList* gokai_list_prepend(GokaiList* list, void* data, GokaiDestroyFunc destroy) {
  if (list == NULL) {
    return gokai_list_alloc(data, destroy);
  }

  GokaiList* head = gokai_list_get_head(list);
  assert(head != NULL);

  GokaiList* item = gokai_list_alloc(data, destroy);

  item->next = head;
  head->prev = item;

  return item;
}

GokaiList* gokai_list_remove(GokaiList* list) {
  if (list == NULL) return NULL;

  GokaiList* head = gokai_list_get_head(list);
  bool is_head = head == list;

  if (list->prev != NULL) {
    list->prev->next = list->next;
  }

  if (list->next != NULL) {
    list->next->prev = list->next;
  }

  list->prev = NULL;
  list->next = NULL;
  return is_head ? NULL : head;
}