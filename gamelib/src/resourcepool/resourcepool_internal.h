#pragma once

#include "external/uthash_wrapper.h"

typedef struct ResourcePoolItem
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;
	struct ResourcePoolItem** head;
	void* payload;
} ResourcePoolItem;

// The create function should allocate and initialise the memory for the item payload.
typedef void (* ResourcePoolCreatePayloadFunc)(ResourcePoolItem* item);

// The destroy function should clea up any resources held in the payload,
// but should NOT free the payload itself.
typedef void (* ResourcePoolDestroyPayloadFunc)(ResourcePoolItem* item);

ResourcePoolItem* ResourcePoolInternal_CreateAndAddRef(
	ResourcePoolItem** head,
	const char* key,
	ResourcePoolCreatePayloadFunc createFunc
);

void ResourcePoolInternal_AddRef(
	ResourcePoolItem* item
);

void ResourcePoolInternal_RemoveRef(
	ResourcePoolItem* item,
	ResourcePoolDestroyPayloadFunc destroyFunc
);