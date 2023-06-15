#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef void* (*GokaiConstructFunc)(const char* first_param, va_list ap);
typedef void (*GokaiDestroyFunc)(void* data);
typedef void (*GokaiRefFunc)(void* data, size_t* count);
typedef void (*GokaiUnrefFunc)(void* data, size_t* count);