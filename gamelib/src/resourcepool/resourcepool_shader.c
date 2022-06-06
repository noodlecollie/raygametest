#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"

struct ResourcePoolShader
{
	ResourcePoolItem* owner;
	Shader shader;
};

static ResourcePoolItem* PresetShaderPoolHead = NULL;

static void CreatePresetShaderPayload(ResourcePoolItem* item)
{
	// TODO: Load shader here

	ResourcePoolShader* payload = (ResourcePoolShader*)MemAlloc(sizeof(ResourcePoolShader));
	item->payload = payload;

	payload->owner = item;
	// TODO: Set shader here
}

static void DestroyShaderPayload(ResourcePoolItem* item)
{
	// TODO
	(void)item;
}

ResourcePoolShader* ResourcePool_LoadPresetShaderAndAddRef(const char* name)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetShaderPoolHead,
		name,
		&CreatePresetShaderPayload
	);

	return item ? (ResourcePoolShader*)item->payload : NULL;
}

ResourcePoolShader* ResourcePool_AddShaderRef(ResourcePoolShader* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePool_RemoveShaderRef(ResourcePoolShader* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroyShaderPayload);
}

const char* ResourcePool_GetShaderKey(ResourcePoolShader* item)
{
	return item ? item->owner->key : NULL;
}
