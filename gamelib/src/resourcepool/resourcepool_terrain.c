#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/terraindescriptor.h"

struct ResourcePoolTerrain
{
	ResourcePoolItem* owner;
	TerrainDescriptor* descriptor;
};

static ResourcePoolItem* TerrainPoolHead = NULL;
static pthread_mutex_t TerrainPoolMutex = PTHREAD_MUTEX_INITIALIZER;

static void CreateTerrainPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	TerrainDescriptor* descriptor = NULL;

	if ( jsonObject )
	{
		descriptor = TerrainDescriptor_LoadFromJSONObject(jsonObject);
	}

	TerrainDescriptor* descriptor = TerrainDescriptor_LoadFromJSONFile(item->key);

	if ( !descriptor )
	{
		TraceLog(
			LOG_DEBUG,
			"RESOURCE POOL: Unable to load terrain descriptor \"%s\" from %s",
			item->key,
			jsonObject ? "JSON" : "file"
		);

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
		&TerrainPoolMutex,
		&TerrainPoolHead,
		path,
		NULL,
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

	ResourcePoolInternal_AddRef(&TerrainPoolMutex, item->owner);
	return item;
}

void ResourcePool_RemoveTerrainRef(ResourcePoolTerrain* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(&TerrainPoolMutex, item->owner, &DestroyTerrainPayload);
}

struct TerrainDescriptor* ResourcePool_GetTerrain(ResourcePoolTerrain* item)
{
	return item ? item->descriptor : NULL;
}

const char* ResourcePool_GetTerrainKey(ResourcePoolTerrain* item)
{
	return item ? item->owner->key : NULL;
}
