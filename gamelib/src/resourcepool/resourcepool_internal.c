#include "resourcepool/resourcepool_internal.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"

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

static inline void AddRef(ResourcePoolItem* item)
{
	++item->refCount;
}

static inline void RemoveRef(ResourcePoolItem* item)
{
	if ( item->refCount > 0 )
	{
		--item->refCount;
	}
}

ResourcePoolItem* ResourcePoolInternal_CreateAndAddRef(
	ResourcePoolItem** head,
	const char* key,
	ResourcePoolCreatePayloadFunc createFunc
)
{
	if ( *head || !createFunc || !key || !(*key) )
	{
		return NULL;
	}

	ResourcePoolItem* item = FindItemByPath(*head, key);

	if ( !item )
	{
		item = CreateItemBase(key);
		(*createFunc)(item);

		if ( !item->payload )
		{
			DestroyItemGeneric(item);
			return NULL;
		}

		AddItemToHash(head, item);
	}

	AddRef(item);

	return item;
}

void ResourcePoolInternal_AddRef(
	ResourcePoolItem* item
)
{
	if ( item )
	{
		AddRef(item);
	}
}

void ResourcePoolInternal_RemoveRef(
	ResourcePoolItem* item,
	ResourcePoolDestroyPayloadFunc destroyFunc
)
{
	if ( !item )
	{
		return;
	}

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
}
