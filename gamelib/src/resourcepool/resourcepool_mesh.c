#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "presets/presetmeshes.h"

struct ResourcePoolMesh
{
	ResourcePoolItem* owner;
	Mesh mesh;
};

static ResourcePoolItem* PresetMeshPoolHead = NULL;

static void CreatePresetMeshPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	if ( jsonObject )
	{
		TraceLog(LOG_ERROR, "RESOURCE POOL: Loading preset mesh from JSON object is not supported");
		return;
	}

	Mesh mesh = PresetMeshes_Create(item->key);

	if ( mesh.vertexCount == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to create preset mesh \"%s\"", item->key);
		return;
	}

	ResourcePoolMesh* payload = (ResourcePoolMesh*)MemAlloc(sizeof(ResourcePoolMesh));
	item->payload = payload;

	payload->owner = item;
	payload->mesh = mesh;
}

static void DestroyMeshPayload(ResourcePoolItem* item)
{
	ResourcePoolMesh* payload = (ResourcePoolMesh*)item->payload;

	if ( payload )
	{
		UnloadMesh(payload->mesh);
	}
}

ResourcePoolMesh* ResourcePool_LoadPresetMeshAndAddRef(const char* name)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetMeshPoolHead,
		name,
		NULL,
		&CreatePresetMeshPayload
	);

	return item ? (ResourcePoolMesh*)item->payload : NULL;
}

ResourcePoolMesh* ResourcePool_AddMeshRef(ResourcePoolMesh* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePool_RemoveMeshRef(ResourcePoolMesh* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroyMeshPayload);
}

Mesh* ResourcePool_GetMesh(ResourcePoolMesh* item)
{
	return item ? &item->mesh : NULL;
}

const char* ResourcePool_GetMeshKey(ResourcePoolMesh* item)
{
	return item ? item->owner->key : NULL;
}
