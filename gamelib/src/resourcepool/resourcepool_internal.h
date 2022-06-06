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

typedef void (* ResourcePoolCreatePayloadFunc)(ResourcePoolItem* item);
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
