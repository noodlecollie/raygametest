#include "resourcepool/resourcepool_internal.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"

static inline ResourcePoolItemNew* FindItemByPath(ResourcePoolItemNew* head, const char* path)
{
	ResourcePoolItemNew* item = NULL;
	HASH_FIND_STR(head, path, item);
	return item;
}

static inline ResourcePoolItemNew* CreateItemBase(const char* path)
{
	ResourcePoolItemNew* item = (ResourcePoolItemNew*)MemAlloc(sizeof(ResourcePoolItemNew));
	item->key = DuplicateString(path);
	return item;
}

static inline void DestroyItemGeneric(ResourcePoolItemNew* item)
{
	if ( item->payload )
	{
		MemFree(item->payload);
	}

	MemFree(item->key);
	MemFree(item);
}

static inline void AddItemToHash(ResourcePoolItemNew** head, ResourcePoolItemNew* item)
{
	item->head = head;
	HASH_ADD_STR(*item->head, key, item);
}

static inline void RemoveItemFromHash(ResourcePoolItemNew* item)
{
	HASH_DEL(*item->head, item);
}

static inline void AddRef(ResourcePoolItemNew* item)
{
	++item->refCount;
}

static inline void RemoveRef(ResourcePoolItemNew* item)
{
	if ( item->refCount > 0 )
	{
		--item->refCount;
	}
}

ResourcePoolItemNew* ResourcePoolInternal_CreateAndAddRef(
	ResourcePoolItemNew** head,
	const char* key,
	ResourcePoolCreatePayloadFunc createFunc
)
{
	if ( *head || !createFunc || !key || !(*key) )
	{
		return NULL;
	}

	ResourcePoolItemNew* item = FindItemByPath(*head, key);

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
	ResourcePoolItemNew* item
)
{
	if ( item )
	{
		AddRef(item);
	}
}

void ResourcePoolInternal_RemoveRef(
	ResourcePoolItemNew* item,
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
