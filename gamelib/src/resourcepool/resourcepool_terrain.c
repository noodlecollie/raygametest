#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/terraindescriptor.h"

struct ResourcePoolTerrain
{
	ResourcePoolItem* owner;
	TerrainDescriptor* descriptor;
};

static ResourcePoolItem* PoolHead = NULL;

static void CreateTerrainPayload(ResourcePoolItem* item)
{
	TerrainDescriptor* descriptor = TerrainDescriptor_LoadFromJSON(item->key);

	if ( !descriptor )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to load terrain descriptor \"%s\"", item->key);
		return;
	}

	ResourcePoolTerrain* payload = (ResourcePoolTerrain*)MemAlloc(sizeof(ResourcePoolTerrain));
	item->payload = payload;

	payload->owner = item;
	payload->descriptor = descriptor;
}

static void DestroyTerrainPayload(ResourcePoolItem* item)
{
	ResourcePoolTerrain* payload = (ResourcePoolTerrain*)item->payload;

	if ( payload )
	{
		TerrainDescriptor_Destroy(payload->descriptor);
	}
}

ResourcePoolTerrain* ResourcePool_LoadTerrainAndAddRef(const char* path)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&PoolHead,
		path,
		&CreateTerrainPayload
	);

	return item ? (ResourcePoolTerrain*)item->payload : NULL;
}

ResourcePoolTerrain* ResourcePool_AddTerrainRef(ResourcePoolTerrain* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePool_RemoveTerrainRef(ResourcePoolTerrain* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroyTerrainPayload);
}

struct TerrainDescriptor* ResourcePool_GetTerrain(ResourcePoolTerrain* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePool_GetTerrainKey(ResourcePoolTerrain* item)
{
	return item ? item->owner->key : NULL;
}
