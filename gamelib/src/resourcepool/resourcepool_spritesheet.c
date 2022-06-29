#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/spritesheetdescriptor.h"

struct ResourcePoolSpriteSheet
{
	ResourcePoolItem* owner;
	SpriteSheetDescriptor* descriptor;
};

static ResourcePoolItem* SpriteSheetPoolHead = NULL;
static pthread_mutex_t SpriteSheetPoolMutex = PTHREAD_MUTEX_INITIALIZER;

static void CreateSpriteSheetPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	if ( jsonObject )
	{
		TraceLog(LOG_WARNING, "RESOURCE POOL: Loading sprite sheet directly from JSON object is not yet implemented");
		return;
	}

	SpriteSheetDescriptor* descriptor = SpriteSheetDescriptor_LoadFromJSONFile(item->key);

	if ( !descriptor )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to load sprite sheet descriptor \"%s\"", item->key);
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

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetAndAddRef(const char* path)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&SpriteSheetPoolMutex,
		&SpriteSheetPoolHead,
		path,
		NULL,
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

	ResourcePoolInternal_AddRef(&SpriteSheetPoolMutex, item->owner);
	return item;
}

void ResourcePool_RemoveSpriteSheetRef(ResourcePoolSpriteSheet* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(&SpriteSheetPoolMutex, item->owner, &DestroySpriteSheetPayload);
}

struct SpriteSheetDescriptor* ResourcePool_GetSpriteSheet(ResourcePoolSpriteSheet* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePool_GetSpriteSheetKey(ResourcePoolSpriteSheet* item)
{
	return item ? item->owner->key : NULL;
}
