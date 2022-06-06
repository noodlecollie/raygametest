#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/spritesheetdescriptor.h"

struct ResourcePoolSpriteSheet
{
	ResourcePoolItemNew* owner;
	SpriteSheetDescriptor* descriptor;
};

static ResourcePoolItemNew* PoolHead = NULL;

static void CreateSpriteSheetPayload(ResourcePoolItemNew* item)
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

static void DestroySpriteSheetPayload(ResourcePoolItemNew* item)
{
	ResourcePoolSpriteSheet* payload = (ResourcePoolSpriteSheet*)item->payload;

	if ( payload )
	{
		SpriteSheetDescriptor_Destroy(payload->descriptor);
	}
}

ResourcePoolSpriteSheet* ResourcePool_LoadSpriteSheetAndAddRef(const char* path)
{
	ResourcePoolItemNew* item = ResourcePoolInternal_CreateAndAddRef(
		&PoolHead,
		path,
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
