#include "resourcepool/resourcepoolnew.h"
#include "resourcepool/resourcepoolnew_internal.h"
#include "descriptor/spritesheetdescriptor.h"

struct ResourcePoolSpriteSheetNew
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

	ResourcePoolSpriteSheetNew* payload = (ResourcePoolSpriteSheetNew*)MemAlloc(sizeof(ResourcePoolSpriteSheetNew));
	item->payload = payload;

	payload->owner = item;
	payload->descriptor = descriptor;
}

static void DestroySpriteSheetPayload(ResourcePoolItemNew* item)
{
	ResourcePoolSpriteSheetNew* payload = (ResourcePoolSpriteSheetNew*)item->payload;

	if ( payload )
	{
		SpriteSheetDescriptor_Destroy(payload->descriptor);
	}
}

ResourcePoolSpriteSheetNew* ResourcePoolNew_LoadSpriteSheetAndAddRef(const char* path)
{
	ResourcePoolItemNew* item = ResourcePoolInternal_CreateAndAddRef(
		&PoolHead,
		path,
		&CreateSpriteSheetPayload
	);

	return item ? (ResourcePoolSpriteSheetNew*)item->payload : NULL;
}

ResourcePoolSpriteSheetNew* ResourcePoolNew_AddSpriteSheetRef(ResourcePoolSpriteSheetNew* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePoolNew_RemoveSpriteSheetRef(ResourcePoolSpriteSheetNew* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroySpriteSheetPayload);
}

struct SpriteSheetDescriptor* ResourcePoolNew_GetSpriteSheet(ResourcePoolSpriteSheetNew* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePoolNew_GetSpriteSheetKey(ResourcePoolSpriteSheetNew* item)
{
	return item ? item->owner->key : NULL;
}
