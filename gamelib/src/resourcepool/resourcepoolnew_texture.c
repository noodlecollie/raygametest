#include "resourcepool/resourcepoolnew.h"
#include "resourcepool/resourcepoolnew_internal.h"
#include "presets/presettextures.h"

struct ResourcePoolTextureNew
{
	ResourcePoolItemNew* owner;
	Texture2D texture;
};

static ResourcePoolItemNew* TexturePoolHead = NULL;
static ResourcePoolItemNew* PresetTexturePoolHead = NULL;

static bool LocalLoadTexture(Texture2D* texture, const char* path)
{
	*texture = LoadTexture(path);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to create texture for %s", path);
		return false;
	}

	return true;
}

static bool LocalLoadPresetTexture(Texture2D* texture, const char* name)
{
	*texture = PresetTextures_Create(name);

	if ( texture->id == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to create preset texture \"%s\"", name);
		return false;
	}

	return true;
}

static void CreateTexturePayloadDelegated(ResourcePoolItemNew* item, bool (* createFunc)(Texture2D*, const char*))
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

	ResourcePoolTextureNew* payload = (ResourcePoolTextureNew*)MemAlloc(sizeof(ResourcePoolTextureNew));
	item->payload = payload;

	payload->owner = item;
	payload->texture = texture;
}

static void CreateTexturePayload(ResourcePoolItemNew* item)
{
	CreateTexturePayloadDelegated(item, &LocalLoadTexture);
}

static void CreatePresetTexturePayload(ResourcePoolItemNew* item)
{
	CreateTexturePayloadDelegated(item, &LocalLoadPresetTexture);
}

static void DestroyTexturePayload(ResourcePoolItemNew* item)
{
	ResourcePoolTextureNew* payload = (ResourcePoolTextureNew*)item->payload;

	if ( payload )
	{
		UnloadTexture(payload->texture);
	}
}

ResourcePoolTextureNew* ResourcePoolNew_LoadTextureAndAddRef(const char* path)
{
	ResourcePoolItemNew* item = ResourcePoolInternal_CreateAndAddRef(
		&TexturePoolHead,
		path,
		&CreateTexturePayload
	);

	return item ? (ResourcePoolTextureNew*)item->payload : NULL;
}

ResourcePoolTextureNew* ResourcePoolNew_LoadPresetTextureAndAddRef(const char* name)
{
	ResourcePoolItemNew* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetTexturePoolHead,
		name,
		&CreatePresetTexturePayload
	);

	return item ? (ResourcePoolTextureNew*)item->payload : NULL;
}

ResourcePoolTextureNew* ResourcePoolNew_AddTextureRef(ResourcePoolTextureNew* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePoolNew_RemoveTextureRef(ResourcePoolTextureNew* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroyTexturePayload);
}

Texture2D* ResourcePoolNew_GetTexture(ResourcePoolTextureNew* item)
{
	return item ? &item->texture : NULL;
}

const char* ResourcePoolNew_GetTextureFileKey(ResourcePoolTextureNew* item)
{
	return item ? item->owner->key : NULL;
}
