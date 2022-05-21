#include "resourcepool.h"
#include "external/uthash_wrapper.h"
#include "gamelib/gameutil.h"
#include "descriptor/spritesheetdescriptor.h"

typedef struct ResourcePoolItem
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;

	void* payload;
} ResourcePoolItem;

struct ResourcePoolTexture
{
	ResourcePoolItem* owner;
	Texture2D texture;
};

typedef struct ResourcePool
{
	ResourcePoolItem* textures;
} ResourcePool;

static ResourcePool Pool;

static inline ResourcePoolItem* FindItemByPath(ResourcePoolItem* head, const char* path)
{
	ResourcePoolItem* item = NULL;
	HASH_FIND_STR(head, path, item);
	return item;
}

static inline ResourcePoolItem* CreateItem(const char* path)
{
	ResourcePoolItem* item = (ResourcePoolItem*)MemAlloc(sizeof(ResourcePoolItem));
	item->key = DuplicateString(path);
	return item;
}

static inline void DestroyItem(ResourcePoolItem* item)
{
	MemFree(item->key);
	MemFree(item);
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

static bool LocalLoadTexture(Texture2D* texture, const char* path)
{
	*texture = LoadTexture(path);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCEPOOL: Unable to create texture for %s", path);
		return false;
	}

	// Allow these to be customised in future?
	// We'd be getting more into material territory then.
	// For now, just let the user of the texture override
	// these manually later, if they want to. These values
	// are good enough defaults.
	SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
	SetTextureWrap(*texture, TEXTURE_WRAP_CLAMP);

	return true;
}

static void CreateTexturePayload(ResourcePoolItem* item)
{
	Texture2D texture = { 0 };

	if ( !LocalLoadTexture(&texture, item->key) )
	{
		return;
	}

	ResourcePoolTexture* payload = (ResourcePoolTexture*)MemAlloc(sizeof(ResourcePoolTexture));
	item->payload = payload;

	payload->owner = item;
	payload->texture = texture;
}

static inline void DestroyTexturePayload(ResourcePoolItem* item)
{
	ResourcePoolTexture* payload = (ResourcePoolTexture*)item->payload;

	if ( payload )
	{
		UnloadTexture(payload->texture);
		MemFree(payload);
	}

	item->payload = NULL;
}

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path)
{
	if ( !path || !(*path) )
	{
		return NULL;
	}

	ResourcePoolItem* item = FindItemByPath(Pool.textures, path);

	if ( !item )
	{
		item = CreateItem(path);
		CreateTexturePayload(item);

		if ( !item->payload )
		{
			DestroyItem(item);
			return NULL;
		}

		// This must be here because the head pointer must be mutable.
		HASH_ADD_STR(Pool.textures, key, item);
	}

	AddRef(item);

	return (ResourcePoolTexture*)item->payload;
}

ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return NULL;
	}

	AddRef(item->owner);

	return item;
}

void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return;
	}

	RemoveRef(item->owner);

	if ( item->owner->refCount < 1 )
	{
		ResourcePoolItem* owner = item->owner;
		HASH_DEL(Pool.textures, owner);

		DestroyTexturePayload(owner);
		DestroyItem(owner);
	}
}

Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item)
{
	return item ? &item->texture : NULL;
}

const char* ResourcePool_GetTextureFilePath(ResourcePoolTexture* item)
{
	return item ? item->owner->key : NULL;
}
