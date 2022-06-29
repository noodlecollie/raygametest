#include <stddef.h>
#include "descriptor/entitygroupdescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"
#include "external/cJSON_wrapper.h"
#include "descriptor/descriptorutil.h"

#define SUPPORTED_VERSION 1

typedef enum
{
	ITEMTYPE_SPRITESHEET,
	ITEMTYPE_TERRAIN
} ItemType;

struct EntityGroupItem
{
	EntityGroupItem* prev;
	EntityGroupItem* next;
	ItemType itemType;
	void* item;
};

struct EntityGroupDescriptor
{
	char* name;
	EntityGroupItem* head;
	EntityGroupItem* tail;
};

static void DestroyItem(EntityGroupItem* item)
{
	MemFree(item);
}

static void LoadEntity(const char* filePath, cJSON* ent, EntityGroupDescriptor* descriptor)
{
	// TODO
	(void)filePath;
	(void)ent;
	(void)descriptor;
}

static void LoadEntities(const char* filePath, cJSON* content, EntityGroupDescriptor* descriptor)
{
	cJSON* entities = cJSONWrapper_GetObjectItemOfType(content, "entities", cJSON_Array);

	if ( !entities )
	{
		return;
	}

	for ( cJSON* child = entities->child; child; child = child->next )
	{
		LoadEntity(filePath, child, descriptor);
	}
}

static EntityGroupDescriptor* Load(const char* filePath, cJSON* root)
{
	EntityGroupDescriptor* descriptor = NULL;
	cJSON* content = Descriptor_GetContent(root, "entitygroup", SUPPORTED_VERSION);

	do
	{
		if ( content )
		{
			cJSON* nameField = cJSONWrapper_GetObjectItemOfType(content, "name", cJSON_String);

			if ( !nameField || !nameField->valuestring || !(*nameField->valuestring) )
			{
				TraceLog(LOG_ERROR, "ENTITY GROUP DESCRIPTOR: [%s] No name was specified for entity group", filePath);
				break;
			}

			descriptor = (EntityGroupDescriptor*)MemAlloc(sizeof(EntityGroupDescriptor));
			descriptor->name = DuplicateString(nameField->valuestring);

			LoadEntities(filePath, content, descriptor);

			TraceLog(LOG_INFO, "ENTITY GROUP DESCRIPTOR: [%s] Loaded successfully", filePath);
		}
		else
		{
			TraceLog(LOG_ERROR, "ENTITY GROUP DESCRIPTOR: [%s] Did not describe a version %d entity group", filePath, SUPPORTED_VERSION);
		}
	}
	while ( false );

	return descriptor;
}

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONFile(const char* filePath)
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

	EntityGroupDescriptor* descriptor = Load(filePath, root);

	cJSON_Delete(root);
	return descriptor;
}

EntityGroupDescriptor* EntityGroupDescriptor_LoadFromJSONObject(cJSON* root)
{
	if ( !root )
	{
		return NULL;
	}

	return Load("local object", root);
}

void EntityGroupDescriptor_Destroy(EntityGroupDescriptor* descriptor)
{
	if ( !descriptor )
	{
		return;
	}

	if ( descriptor->name )
	{
		MemFree(descriptor->name);
	}

	EntityGroupItem* item = descriptor->head;
	descriptor->head = NULL;
	descriptor->tail = NULL;

	while ( item )
	{
		EntityGroupItem* next = item->next;

		DestroyItem(item);

		item = next;
	}

	MemFree(descriptor);
}
