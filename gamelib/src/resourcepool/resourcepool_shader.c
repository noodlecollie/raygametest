#include "resourcepool/resourcepool.h"
#include "resourcepool/resourcepool_internal.h"
#include "presets/presetshaders.h"

struct ResourcePoolShader
{
	ResourcePoolItem* owner;
	Shader shader;
};

static ResourcePoolItem* PresetShaderPoolHead = NULL;
static pthread_mutex_t PresetShaderPoolMutex = PTHREAD_MUTEX_INITIALIZER;

static void CreatePresetShaderPayload(ResourcePoolItem* item, struct cJSON* jsonObject)
{
	if ( jsonObject )
	{
		TraceLog(LOG_WARNING, "RESOURCE POOL: Loading preset shader directly from JSON object is not yet implemented");
		return;
	}

	Shader shader = PresetShaders_Create(item->key);

	if ( shader.id == 0 )
	{
		TraceLog(LOG_DEBUG, "RESOURCE POOL: Unable to create preset shader \"%s\"", item->key);
		return;
	}

	ResourcePoolShader* payload = (ResourcePoolShader*)MemAlloc(sizeof(ResourcePoolShader));
	item->payload = payload;

	payload->owner = item;
	payload->shader = shader;
}

static void DestroyShaderPayload(ResourcePoolItem* item)
{
	ResourcePoolShader* payload = (ResourcePoolShader*)item->payload;

	if ( payload )
	{
		UnloadShader(payload->shader);
	}
}

ResourcePoolShader* ResourcePool_LoadPresetShaderAndAddRef(const char* name)
{
	ResourcePoolItem* item = ResourcePoolInternal_CreateAndAddRef(
		&PresetShaderPoolMutex,
		&PresetShaderPoolHead,
		name,
		NULL,
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

	ResourcePoolInternal_AddRef(&PresetShaderPoolMutex, item->owner);
	return item;
}

void ResourcePool_RemoveShaderRef(ResourcePoolShader* item)
{
	if ( !item )
	{
		return;
	}

	ResourcePoolInternal_RemoveRef(&PresetShaderPoolMutex, item->owner, &DestroyShaderPayload);
}

Shader* ResourcePool_GetShader(ResourcePoolShader* item)
{
	return item ? &item->shader : NULL;
}

const char* ResourcePool_GetShaderKey(ResourcePoolShader* item)
{
	return item ? item->owner->key : NULL;
}
