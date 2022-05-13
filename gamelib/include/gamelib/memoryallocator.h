#pragma once

#include <stdlib.h>

void* GameAlloc(size_t size);
void* GameCalloc(size_t num, size_t size);
void* GameRealloc(void* ptr, size_t size);
void GameFree(void* ptr);

#define RL_MALLOC(sz) malloc(sz)
#define RL_CALLOC(n,sz) calloc(n,sz)
#define RL_REALLOC(ptr,sz) realloc(ptr,sz)
#define RL_FREE(ptr) free(ptr)
