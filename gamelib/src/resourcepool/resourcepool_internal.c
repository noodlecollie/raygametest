#include "resourcepool/resourcepool_internal.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"

static inline ResourcePoolItem* FindItemByPath(ResourcePoolItem* head, const char* path)
{
	ResourcePoolItem* item = NULL;
	HASH_FIND_STR(head, path, item);
	return item;
}

static inline ResourcePoolItem* CreateItemBase(const char* path)
{
	ResourcePoolItem* item = (ResourcePoolItem*)MemAlloc(sizeof(ResourcePoolItem));
	item->key = DuplicateString(path);
	return item;
}

static inline void DestroyItemGeneric(ResourcePoolItem* item)
{
	if ( item->payload )
	{
		MemFree(item->payload);
	}

	MemFree(item->key);
	MemFree(item);
}

static inline void AddItemToHash(ResourcePoolItem** head, ResourcePoolItem* item)
{
	item->head = head;
	HASH_ADD_STR(*item->head, key, item);
}

static inline void RemoveItemFromHash(ResourcePoolItem* item)
{
	HASH_DEL(*item->head, item);
}

static ResourcePoolItem* FindOrCreateItem(
	ResourcePoolItem** head,
	const char* key,
	struct cJSON* jsonObject,
	ResourcePoolCreatePayloadFunc createFunc
)
{
	ResourcePoolItem* item = FindItemByPath(*head, key);

	if ( !item )
	{
		item = CreateItemBase(key);
		(*createFunc)(item, jsonObject);

		if ( item->payload )
		{
			AddItemToHash(head, item);
		}
		else
		{
			DestroyItemGeneric(item);
			item = NULL;
		}
	}

	return item;
}

static inline void AddRef(ResourcePoolItem* item)
{
	if ( (item->refCount + 1) < item->refCount )
	{
		// Should never happen, better that we know from a fatal log than a crash
		TraceLog(LOG_FATAL, "RESOURCE POOL: Ref count overflow when adding ref for \"%s\"!", item->key);
		return;
	}

	++item->refCount;
}

static inline void RemoveRef(ResourcePoolItem* item)
{
	if ( item->refCount == 0 )
	{
		// Should never happen, better that we know from a fatal log than a crash
		TraceLog(LOG_FATAL, "RESOURCE POOL: Ref count underflow when removing ref for \"%s\"!", item->key);
		return;
	}

	--item->refCount;
}

ResourcePoolItem* ResourcePoolInternal_CreateAndAddRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem** head,
	const char* key,
	struct cJSON* jsonObject,
	ResourcePoolCreatePayloadFunc createFunc
)
{
	if ( !head || !createFunc || !key || !(*key) )
	{
		return NULL;
	}

	THREADING_LOCK_MUTEX(mutex);

	ResourcePoolItem* item = FindOrCreateItem(head, key, jsonObject, createFunc);

	if ( item )
	{
		AddRef(item);
	}

	THREADING_UNLOCK_MUTEX(mutex);
	return item;
}

void ResourcePoolInternal_AddRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem* item
)
{
	if ( !item )
	{
		return;
	}

	THREADING_LOCK_MUTEX(mutex);
	AddRef(item);
	THREADING_UNLOCK_MUTEX(mutex);
}

void ResourcePoolInternal_RemoveRef(
	pthread_mutex_t* mutex,
	ResourcePoolItem* item,
	ResourcePoolDestroyPayloadFunc destroyFunc
)
{
	if ( !item )
	{
		return;
	}

	THREADING_LOCK_MUTEX(mutex);

	RemoveRef(item);

	if ( item->refCount < 1 )
	{
		RemoveItemFromHash(item);

		if ( destroyFunc )
		{
			(*destroyFunc)(item);
		}

		DestroyItemGeneric(item);
	}

	THREADING_UNLOCK_MUTEX(mutex);
}
