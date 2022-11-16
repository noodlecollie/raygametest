#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "descriptor/terraindescriptor.h"

struct ResourcePoolTerrain
{
	ResourcePoolItem* owner;
	TerrainDescriptor* descriptor;
};

static ResourcePoolItem* TerrainPoolHead = NULL;

static void CreateTerrainPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	TerrainDescriptor* descriptor = NULL;

	if ( jsonObject )
	{
		descriptor = TerrainDescriptor_LoadFromJSONObject(jsonObject);
	}
	else
	{
		descriptor = TerrainDescriptor_LoadFromJSONFile(item->key);
	}

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

ResourcePoolTerrain* ResourcePool_LoadTerrainFromFileAndAddRef(const char* path)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&TerrainPoolHead,
		path,
		NULL,
		&CreateTerrainPayload
	);

	return item ? (ResourcePoolTerrain*)item->payload : NULL;
}

ResourcePoolTerrain* ResourcePool_LoadTerrainFromJSONAndAddRef(const char* key, struct cJSON* root)
{
	// Check this first, otherwise a file load would be attempted.
	if ( !root )
	{
		return NULL;
	}

	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&TerrainPoolHead,
		key,
		root,
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
