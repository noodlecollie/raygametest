#include <stddef.h>
#include "descriptor/entitygroupdescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "external/cJSON_wrapper.h"
#include "descriptor/descriptorutil.h"

#define SUPPORTED_VERSION 1

struct EntityGroupDescriptor
{
	int dummy;
};

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSON(const char* filePath)
{
	if ( !filePath || !(*filePath) )
	{
		return NULL;
	}

	cJSON* root = cJSONWrapper_ParseFile(filePath);

	if ( !root )
	{
		TraceLog(LOG_ERROR, "TERRAIN DESCRIPTOR: [%s] Could not load file", filePath);
		return NULL;
	}

	EntityGroupDescriptor* descriptor = NULL;
	cJSON* content = Descriptor_GetContent(root, "entitygroup", SUPPORTED_VERSION);

	if ( content )
	{
		descriptor = (EntityGroupDescriptor*)MemAlloc(sizeof(EntityGroupDescriptor));

		TraceLog(LOG_INFO, "ENTITY GROUP DESCRIPTOR: [%s] Loaded successfully", filePath);
	}
	else
	{
		TraceLog(LOG_ERROR, "ENTITY GROUP DESCRIPTOR: [%s] File did not describe a version %d entity group", filePath, SUPPORTED_VERSION);
	}

	cJSON_Delete(root);
	return descriptor;
}

void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor)
{
	if ( !descriptor )
	{
		return;
	}

	MemFree(descriptor);
}
