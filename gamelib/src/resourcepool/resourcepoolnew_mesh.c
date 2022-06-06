#include "resourcepool/resourcepoolnew.h"
#include "resourcepool/resourcepoolnew_internal.h"
#include "presets/presetmeshes.h"

struct ResourcePoolMeshNew
{
	ResourcePoolItemNew* owner;
	Mesh mesh;
};

ResourcePoolItemNew* PresetMeshPoolHead = NULL;

static void CreatePresetMeshPayload(ResourcePoolItemNew* item)
{
	Mesh mesh = PresetMeshes_Create(item->key);

	if ( mesh.vertexCount == 0 )
	{
		return;
	}

	ResourcePoolMeshNew* payload = (ResourcePoolMeshNew*)MemAlloc(sizeof(ResourcePoolMeshNew));
	item->payload = payload;

	payload->owner = item;
	payload->mesh = mesh;
}

static void DestroyMeshPayload(ResourcePoolItemNew* item)
{
	ResourcePoolMeshNew* payload = (ResourcePoolMeshNew*)item->payload;

	if ( payload )
	{
		UnloadMesh(payload->mesh);
	}
}

ResourcePoolMeshNew* ResourcePoolNew_LoadPresetMeshAndAddRef(const char* name)
{
	ResourcePoolItemNew* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetMeshPoolHead,
		name,
		&CreatePresetMeshPayload
	);

	return item ? (ResourcePoolMeshNew*)item->payload : NULL;
}

ResourcePoolMeshNew* ResourcePoolNew_AddMeshRef(ResourcePoolMeshNew* item)
{
	if ( !item )
	{
		return NULL;
	}

	ResourcePoolInternal_AddRef(item->owner);
	return item;
}

void ResourcePoolNew_RemoveMeshRef(ResourcePoolMeshNew* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(item->owner, &DestroyMeshPayload);
}

Mesh* ResourcePoolNew_GetMesh(ResourcePoolMeshNew* item)
{
	return item ? &item->mesh : NULL;
}

const char* ResourcePoolNew_GetMeshKey(ResourcePoolMeshNew* item)
{
	return item ? item->owner->key : NULL;
}
