#include "resourcepool.h"
#include "external/uthash_wrapper.h"
#include "gamelib/gameutil.h"
#include "descriptor/spritesheetdescriptor.h"
#include "presets/presetmeshes.h"

typedef struct ResourcePoolItem
{
	UT_hash_handle hh;
	char* key;
	size_t refCount;
	struct ResourcePoolItem** head;
	void* payload;
} ResourcePoolItem;

struct ResourcePoolTexture
{
	ResourcePoolItem* owner;
	Texture2D texture;
};

struct ResourcePoolSpriteSheet
{
	ResourcePoolItem* owner;
	SpriteSheetDescriptor* descriptor;
};

struct ResourcePoolMesh
{
	ResourcePoolItem* owner;
	Mesh mesh;
};

typedef struct ResourcePool
{
	ResourcePoolItem* textures;
	ResourcePoolItem* spriteSheets;
	ResourcePoolItem* presetMeshes;
} ResourcePool;

typedef void (* CreatePayloadFunc)(ResourcePoolItem* item);
typedef void (* DestroyPayloadFunc)(ResourcePoolItem* item);

static ResourcePool Pool;

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

static inline void DestroyItem(ResourcePoolItem* item)
{
	if ( item->payload )
	{
		MemFree(item->payload);
	}

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

static inline void AddItemToHash(ResourcePoolItem** head, ResourcePoolItem* item)
{
	ResourcePoolItem* pool = *head;
	HASH_ADD_STR(pool, key, item);
	*head = pool;

	item->head = head;
}

static inline void RemoveItemFromHash(ResourcePoolItem* item)
{
	ResourcePoolItem* pool = *item->head;
	HASH_DEL(pool, item);
	*item->head = pool;
}

static ResourcePoolItem* CreateItemDelegated(ResourcePoolItem** head, const char* key, CreatePayloadFunc createFunc)
{
	if ( !key || !(*key) )
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
			DestroyItem(item);
			return NULL;
		}

		AddItemToHash(head, item);
	}

	AddRef(item);

	return item;
}

static void RemoveRefFromItemDelegated(ResourcePoolItem* item, DestroyPayloadFunc destroyFunc)
{
	RemoveRef(item);

	if ( item->refCount < 1 )
	{
		RemoveItemFromHash(item);

		(*destroyFunc)(item);
		DestroyItem(item);
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

static void DestroyTexturePayload(ResourcePoolItem* item)
{
	ResourcePoolTexture* payload = (ResourcePoolTexture*)item->payload;

	if ( payload )
	{
		UnloadTexture(payload->texture);
	}
}

static void CreateMeshPayload(ResourcePoolItem* item)
{
	Mesh mesh = PresetMeshes_Create(item->key);

	if ( mesh.vertexCount == 0 )
	{
		return;
	}

	UploadMesh(&mesh, false);

	ResourcePoolMesh* payload = (ResourcePoolMesh*)MemAlloc(sizeof(ResourcePoolMesh));
	item->payload = payload;

	payload->owner = item;
	payload->mesh = mesh;
}

static void DestroyMeshPayload(ResourcePoolItem* item)
{
	ResourcePoolMesh* payload = (ResourcePoolMesh*)item->payload;

	if ( payload )
	{
		UnloadMesh(payload->mesh);
	}
}

static void CreateSpriteSheetPayload(ResourcePoolItem* item)
{
	SpriteSheetDescriptor* descriptor = SpriteSheetDescriptor_LoadFromJSON(item->key);

	if ( !descriptor )
	{
		return;
	}

	ResourcePoolSpriteSheet* payload = (ResourcePoolSpriteSheet*)MemAlloc(sizeof(ResourcePoolSpriteSheet));
	item->payload = payload;

	payload->owner = item;
	payload->descriptor = descriptor;
}

static void DestroySpriteSheetPayload(ResourcePoolItem* item)
{
	ResourcePoolSpriteSheet* payload = (ResourcePoolSpriteSheet*)item->payload;

	if ( payload )
	{
		SpriteSheetDescriptor_Destroy(payload->descriptor);
	}
}

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path)
{
	ResourcePoolItem* item = CreateItemDelegated(&Pool.textures, path, &CreateTexturePayload);
	return item ? (ResourcePoolTexture*)item->payload : NULL;
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

	RemoveRefFromItemDelegated(item->owner, &DestroyTexturePayload);
}

Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item)
{
	return item ? &item->texture : NULL;
}

const char* ResourcePool_GetTextureFilePath(ResourcePoolTexture* item)
{
	return item ? item->owner->key : NULL;
}

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetAndAddRef(const char* path)
{
	ResourcePoolItem* item = CreateItemDelegated(&Pool.spriteSheets, path, &CreateSpriteSheetPayload);
	return item ? (ResourcePoolSpriteSheet*)item->payload : NULL;
}

ResourcePoolSpriteSheet* ResourcePool_AddSpriteSheetRef(ResourcePoolSpriteSheet* item)
{
	if ( !item )
	{
		return NULL;
	}

	AddRef(item->owner);

	return item;
}

void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item)
{
	if ( !item )
	{
		return;
	}

	RemoveRefFromItemDelegated(item->owner, &DestroySpriteSheetPayload);
}

struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePool_GetSpriteSheetFilePath(ResourcePoolSpriteSheet* item)
{
	return item ? item->owner->key : NULL;
}

ResourcePoolMesh* ResourcePool_LoadPresetMesh(const char* name)
{
	ResourcePoolItem* item = CreateItemDelegated(&Pool.presetMeshes, name, &CreateMeshPayload);
	return item ? (ResourcePoolMesh*)item->payload : NULL;
}

void ResourcePool_RemoveMeshRef(ResourcePoolMesh* item)
{
	if ( !item )
	{
		return;
	}

	RemoveRefFromItemDelegated(item->owner, &DestroyMeshPayload);
}

Mesh* ResourcePool_GetMesh(ResourcePoolMesh* item)
{
	return item ? &item->mesh : NULL;
}

const char* ResourcePool_GetMeshPresetName(ResourcePoolMesh* item)
{
	return item ? item->owner->key : NULL;
}
