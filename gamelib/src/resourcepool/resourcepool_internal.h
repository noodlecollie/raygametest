#pragma once

#include "external/uthash_wrapper.h"
#include "threading.h"

struct cJSON;

typedef struct ResourcePoolItem
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;
	struct ResourcePoolItem** head;
	void* payload;
} ResourcePoolItem;

// The create function should allocate and initialise the memory for the item payload.
// If the JSON object is valid then it should be treated as being the root object in
// the descriptor file; if it is not valid, the item's key should be treated as the
// path to the descriptor file on disk.
typedef void (* ResourcePoolCreatePayloadFunc)(ResourcePoolItem* item, struct cJSON* jsonObject);

// The destroy function should clea up any resources held in the payload,
// but should NOT free the payload itself.
typedef void (* ResourcePoolDestroyPayloadFunc)(ResourcePoolItem* item);

ResourcePoolItem* ResourcePoolInternal_CreateAndAddRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem** head,
	const char* key,
	struct cJSON* jsonObject,
	ResourcePoolCreatePayloadFunc createFunc
);

void ResourcePoolInternal_AddRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem* item
);

void ResourcePoolInternal_RemoveRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem* item,
	ResourcePoolDestroyPayloadFunc destroyFunc
);
