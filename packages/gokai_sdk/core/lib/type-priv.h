#pragma once

#include <gokai/type.h>

struct _GokaiTypeInstance {
  GokaiTypeId id;
  GokaiTypeRegistry* registry;
  size_t ref_count;
};