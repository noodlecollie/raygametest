#pragma once

#include <stddef.h>

void* GameAlloc(size_t size);
void* GameCalloc(size_t num, size_t size);
void* GameRealloc(void* ptr, size_t size);
void GameFree(void* ptr);

#define RL_MALLOC(sz) GameAlloc(sz)
#define RL_CALLOC(n,sz) GameCalloc(n,sz)
#define RL_REALLOC(ptr,sz) GameRealloc(ptr,sz)
#define RL_FREE(ptr) GameFree(ptr)
