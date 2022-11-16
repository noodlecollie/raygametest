#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/spritesheetdescriptor.h"

struct ResourcePoolSpriteSheet
{
	ResourcePoolItem* owner;
	SpriteSheetDescriptor* descriptor;
};

static ResourcePoolItem* SpriteSheetPoolHead = NULL;

static void CreateSpriteSheetPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	SpriteSheetDescriptor* descriptor = NULL;

	if ( jsonObject )
	{
		descriptor = SpriteSheetDescriptor_LoadFromJSONObject(jsonObject);
		return;
	}
	else
	{
		descriptor = SpriteSheetDescriptor_LoadFromJSONFile(item->key);
	}

	if ( !descriptor )
	{
		TraceLog(
			LOG_DEBUG,
			"RESOURCE POOL: Unable to load sprite sheet descriptor \"%s\" from %s",
			item->key,
			jsonObject ? "JSON" : "file"
		);

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

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetFromFileAndAddRef(const char* path)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&SpriteSheetPoolHead,
		path,
		NULL,
		&CreateSpriteSheetPayload
	);

	return item ? (ResourcePoolSpriteSheet*)item->payload : NULL;
}

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetFromJSONAndAddRef(const char* key, struct cJSON* root)
{
	// Check this first, otherwise a file load would be attempted.
	if ( !root )
	{
		return NULL;
	}

	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&SpriteSheetPoolHead,
		key,
		root,
		&CreateSpriteSheetPayload
	);

	return item ? (ResourcePoolSpriteSheet*)item->payload : NULL;
}

ResourcePoolSpriteSheet* ResourcePool_AddSpriteSheetRef(ResourcePoolSpriteSheet* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroySpriteSheetPayload);
}

struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePool_GetSpriteSheetKey(ResourcePoolSpriteSheet* item)
{
	return item ? item->owner->key : NULL;
}
