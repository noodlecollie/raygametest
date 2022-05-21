#pragma once

#include "gamelib/memoryallocator.h"

// Removes warning about unreachable code
#define HASH_NONFATAL_OOM 1

// Override these to use our own functions, whose failures are already fatal
#define uthash_malloc(sz) GameAlloc(sz)
#define uthash_free(ptr,sz) GameFree(ptr)

#include "external/uthash/uthash.h"
