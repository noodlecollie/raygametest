#pragma once

#include "external/uthash_wrapper.h"

typedef struct ResourcePoolItemNew
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;
	struct ResourcePoolItemNew** head;
	void* payload;
} ResourcePoolItemNew;

typedef void (* ResourcePoolCreatePayloadFunc)(ResourcePoolItemNew* item);
typedef void (* ResourcePoolDestroyPayloadFunc)(ResourcePoolItemNew* item);

ResourcePoolItemNew* ResourcePoolInternal_CreateAndAddRef(
	ResourcePoolItemNew** head,
	const char* key,
	ResourcePoolCreatePayloadFunc createFunc
);

void ResourcePoolInternal_AddRef(
	ResourcePoolItemNew* item
);

void ResourcePoolInternal_RemoveRef(
	ResourcePoolItemNew* item,
	ResourcePoolDestroyPayloadFunc destroyFunc
);
