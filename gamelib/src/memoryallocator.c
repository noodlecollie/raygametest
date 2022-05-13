#include "gamelib/memoryallocator.h"
#include "raylib.h"

static inline void* CheckAllocation(const char* funcName, void* ptr, size_t size)
{
	if ( !ptr )
	{
		TraceLog(LOG_FATAL, "%s memory allocation of %zu bytes failed!\n", funcName, size);

		// LOG_FATAL will exit, so this should never be hit.
		return NULL;
	}

	return ptr;
}

void* GameAlloc(size_t size)
{
	return CheckAllocation("GameAlloc", malloc(size), size);
}

void* GameCalloc(size_t num, size_t size)
{
	return CheckAllocation("GameCalloc", calloc(num, size), num * size);
}

void* GameRealloc(void* ptr, size_t size)
{
	return CheckAllocation("GameRealloc", realloc(ptr, size), size);
}

void GameFree(void* ptr)
{
	free(ptr);
}
