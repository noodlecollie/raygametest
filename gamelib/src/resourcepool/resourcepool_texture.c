#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "presets/presettextures.h"

struct ResourcePoolTexture
{
	ResourcePoolItem* owner;
	Texture2D texture;
};

static ResourcePoolItem* TexturePoolHead = NULL;
static pthread_mutex_t TexturePoolMutex = PTHREAD_MUTEX_INITIALIZER;

static ResourcePoolItem* PresetTexturePoolHead = NULL;
static pthread_mutex_t PresetTexturePoolMutex = PTHREAD_MUTEX_INITIALIZER;

static bool LocalLoadTexture(Texture2D* texture, const char* path)
{
	*texture = LoadTexture(path);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_ERROR, "RESOURCE POOL: Unable to create texture for %s", path);
		return false;
	}

	return true;
}

static bool LocalLoadPresetTexture(Texture2D* texture, const char* name)
{
	*texture = PresetTextures_Create(name);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_ERROR, "RESOURCE POOL: Unable to create preset texture \"%s\"", name);
		return false;
	}

	return true;
}

static void CreateTexturePayloadDelegated(ResourcePoolItem* item, bool (* createFunc)(Texture2D*, const char*))
{
	Texture2D texture = { 0 };

	if ( !(*createFunc)(&texture, item->key) )
	{
		return;
	}

	// Allow these to be customised in future?
	// We'd be getting more into material territory then.
	// For now, just let the user of the texture override
	// these manually later, if they want to. These values
	// are good enough defaults.
	SetTextureFilter(texture, TEXTURE_FILTER_POINT);
	SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);

	ResourcePoolTexture* payload = (ResourcePoolTexture*)MemAlloc(sizeof(ResourcePoolTexture));
	item->payload = payload;

	payload->owner = item;
	payload->texture = texture;
}

static void CreateTexturePayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	if ( jsonObject )
	{
		TraceLog(LOG_WARNING, "RESOURCE POOL: Loading texture directly from JSON object is not yet implemented");
		return;
	}

	CreateTexturePayloadDelegated(item, &LocalLoadTexture);
}

static void CreatePresetTexturePayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	if ( jsonObject )
	{
		TraceLog(LOG_WARNING, "RESOURCE POOL: Loading preset texture directly from JSON object is not yet implemented");
		return;
	}

	CreateTexturePayloadDelegated(item, &LocalLoadPresetTexture);
}

static void DestroyTexturePayload(ResourcePoolItem* item)
{
	ResourcePoolTexture* payload = (ResourcePoolTexture*)item->payload;

	if ( payload )
	{
		UnloadTexture(payload->texture);
	}
}

ResourcePoolTexture* ResourcePool_LoadTextureAndAddRef(const char* path)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&TexturePoolMutex,
		&TexturePoolHead,
		path,
		NULL,
		&CreateTexturePayload
	);

	return item ? (ResourcePoolTexture*)item->payload : NULL;
}

ResourcePoolTexture* ResourcePool_LoadPresetTextureAndAddRef(const char* name)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetTexturePoolMutex,
		&PresetTexturePoolHead,
		name,
		NULL,
		&CreatePresetTexturePayload
	);

	return item ? (ResourcePoolTexture*)item->payload : NULL;
}

ResourcePoolTexture* ResourcePool_AddTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return NULL;
	}

	pthread_mutex_t* mutex = item->owner->head == &PresetTexturePoolHead
		? &PresetTexturePoolMutex
		: &TexturePoolMutex;

	ResourcePoolInternal_AddRef(mutex, item->owner);
	return item;
}

void ResourcePool_RemoveTextureRef(ResourcePoolTexture* item)
{
	if ( !item )
	{
		return;
	}

	pthread_mutex_t* mutex = item->owner->head == &PresetTexturePoolHead
		? &PresetTexturePoolMutex
		: &TexturePoolMutex;

	ResourcePoolInternal_RemoveRef(mutex, item->owner, &DestroyTexturePayload);
}

Texture2D* ResourcePool_GetTexture(ResourcePoolTexture* item)
{
	return item ? &item->texture : NULL;
}

const char* ResourcePool_GetTextureFileKey(ResourcePoolTexture* item)
{
	return item ? item->owner->key : NULL;
}
