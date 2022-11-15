#include <stddef.h>
#include <string.h>
#include "descriptor/entitygroupdescriptor.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"
#include "external/cJSON_wrapper.h"
#include "descriptor/descriptorutil.h"
#include "listmacros.h"
#include "resourcepool/resourcepool.h"

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

	char* name;
	size_t index;
	char** tags;
	size_t tagCount;

	ResourcePoolTerrain* terrain;
	ResourcePoolSpriteSheet* spriteSheet;
};

struct EntityGroupDescriptor
{
	char* name;
	size_t count;
	EntityGroupItem* head;
	EntityGroupItem* tail;
};

// Name is in the format "filePath:entityIndex:componentType"
static char* GenerateUniqueResourceName(const char* filePath, size_t index, const char* componentType)
{
	char suffix[32];

	FormatStringBuffer(suffix, sizeof(suffix), ":%zu:%s", index, componentType);

	size_t nameSize = strlen(filePath) + strlen(suffix) + 1;
	char* name = (char*)GameAlloc(nameSize);

	FormatStringBuffer(name, nameSize, "%s%s", filePath, suffix);

	return name;
}

static void DestroyItem(EntityGroupItem* item)
{
	if ( item->terrain )
	{
		ResourcePool_RemoveTerrainRef(item->terrain);
		item->terrain = NULL;
	}

	if ( item->spriteSheet )
	{
		ResourcePool_RemoveSpriteSheetRef(item->spriteSheet);
		item->spriteSheet = NULL;
	}

	if ( item->tags )
	{
		MemFree(item->tags);
	}

	if ( item->name )
	{
		MemFree(item->name);
	}

	MemFree(item);
}

static void LoadTerrainComponent(const char* filePath, cJSON* terrain, EntityGroupItem* item)
{
	switch ( terrain->type )
	{
		case cJSON_String:
		{
			item->terrain = ResourcePool_LoadTerrainFromFileAndAddRef(terrain->valuestring);
			break;
		}

		case cJSON_Object:
		{
			char* terrainKey = GenerateUniqueResourceName(filePath, item->index, "terrain");
			item->terrain = ResourcePool_LoadTerrainFromJSONAndAddRef(terrainKey, terrain);
			GameFree(terrainKey);
			break;
		}

		default:
		{
			TraceLog(
				LOG_WARNING,
				"ENTITY GROUP DESCRIPTOR: [%s] Entity entry %zu (\"%s\") terrain component was not specified as a file path or inline object",
				filePath,
				item->index + 1,
				item->name ? item->name : ""
			);

			return;
		}
	}

	if ( !item->terrain )
	{
		TraceLog(
			LOG_WARNING,
			"ENTITY GROUP DESCRIPTOR: [%s] Entity entry %zu (\"%s\"): could not load terrain component",
			filePath,
			item->index + 1,
			item->name ? item->name : ""
		);
	}
}

static void LoadSpriteSheetComponent(const char* filePath, cJSON* spriteSheet, EntityGroupItem* item)
{
	switch ( spriteSheet->type )
	{
		case cJSON_String:
		{
			item->spriteSheet = ResourcePool_LoadSpriteSheetFromFileAndAddRef(spriteSheet->valuestring);
			break;
		}

		case cJSON_Object:
		{
			char* spriteSheetKey = GenerateUniqueResourceName(filePath, item->index, "spritesheet");
			item->spriteSheet = ResourcePool_LoadSpriteSheetFromJSONAndAddRef(spriteSheetKey, spriteSheet);
			GameFree(spriteSheetKey);
			break;
		}

		default:
		{
			TraceLog(
				LOG_WARNING,
				"ENTITY GROUP DESCRIPTOR: [%s] Entity entry %zu (\"%s\") sprite sheet component was not specified as a file path or inline object",
				filePath,
				item->index + 1,
				item->name ? item->name : ""
			);

			return;
		}
	}

	if ( !item->spriteSheet )
	{
		TraceLog(
			LOG_WARNING,
			"ENTITY GROUP DESCRIPTOR: [%s] Entity entry %zu (\"%s\"): could not load sprite sheet component",
			filePath,
			item->index + 1,
			item->name ? item->name : ""
		);
	}
}

static void LoadTags(cJSON* ent, EntityGroupItem* item)
{
	cJSON* tagsArray = cJSONWrapper_GetObjectItemOfType(ent, "tags", cJSON_Array);

	if ( !tagsArray )
	{
		return;
	}

	size_t count = 0;

	for ( cJSON* child = tagsArray->child; child; child = child->next )
	{
		if ( child->type == cJSON_String && child->valuestring && *child->valuestring )
		{
			++count;
		}
	}

	item->tagCount = count;

	if ( item->tagCount < 1 )
	{
		return;
	}

	item->tags = (char**)MemAlloc((int)(count * sizeof(char*)));

	count = 0;
	for ( cJSON* child = tagsArray->child; child; child = child->next )
	{
		if ( child->type == cJSON_String && child->valuestring && *child->valuestring )
		{
			item->tags[count++] = DuplicateString(child->valuestring);
		}
	}
}

static void LoadEntity(const char* filePath, cJSON* ent, size_t index, EntityGroupDescriptor* descriptor)
{
	if ( ent->type != cJSON_Object )
	{
		TraceLog(LOG_WARNING, "ENTITY GROUP DESCRIPTOR: [%s] Entity entry %zu was not specified as an object, skipping", filePath, index + 1);
		return;
	}

	EntityGroupItem* item = (EntityGroupItem*)MemAlloc(sizeof(EntityGroupItem));

	cJSON* nameJSON = cJSONWrapper_GetObjectItemOfType(ent, "name", cJSON_String);
	const char* entName = nameJSON ? FirstNonWhitespace(nameJSON->valuestring) : NULL;
	const bool hasValidName = entName && *entName;

	if ( hasValidName )
	{
		item->name = DuplicateString(entName);
	}

	item->index = index;

	DBL_LL_ADD_TO_TAIL(item, prev, next, descriptor, head, tail);
	++descriptor->count;

	LoadTags(ent, item);

	cJSON* terrain = cJSON_GetObjectItem(ent, "terrain");

	if ( terrain )
	{
		LoadTerrainComponent(filePath, terrain, item);
	}

	cJSON* spriteSheet = cJSON_GetObjectItem(ent, "spritesheet");

	if ( spriteSheet )
	{
		LoadSpriteSheetComponent(filePath, spriteSheet, item);
	}
}

static void LoadEntities(const char* filePath, cJSON* content, EntityGroupDescriptor* descriptor)
{
	cJSON* entities = cJSONWrapper_GetObjectItemOfType(content, "entities", cJSON_Array);

	if ( !entities )
	{
		return;
	}

	size_t index = 0;
	for ( cJSON* child = entities->child; child; child = child->next, ++index )
	{
		LoadEntity(filePath, child, index, descriptor);
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

			TraceLog(LOG_INFO, "ENTITY GROUP DESCRIPTOR: [%s] Loaded successfully (%zu entities)", filePath, descriptor->count);
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
		TraceLog(LOG_ERROR, "ENTITY GROUP DESCRIPTOR: [%s] Could not load file", filePath);
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

const char* EntityGroupDescriptor_GetName(EntityGroupDescriptor* descriptor)
{
	return descriptor ? descriptor->name : NULL;
}

size_t EntityGroupDescriptor_GetItemCount(const EntityGroupDescriptor* descriptor)
{
	return descriptor ? descriptor->count : 0;
}

EntityGroupItem* EntityGroupDescriptor_GetFirstItem(EntityGroupDescriptor* descriptor)
{
	return descriptor ? descriptor->head : NULL;
}

EntityGroupItem* EntityGroupDescriptor_GetPreviousItem(EntityGroupItem* item)
{
	return item ? item->prev : NULL;
}

EntityGroupItem* EntityGroupDescriptor_GetNextItem(EntityGroupItem* item)
{
	return item ? item->next : NULL;
}

const char* EntityGroupDescriptor_GetItemName(EntityGroupItem* item)
{
	return item ? item->name : NULL;
}

size_t EntityGroupDescriptor_GetItemIndex(EntityGroupItem* item)
{
	return item ? item->index : 0;
}

size_t EntityGroupDescriptor_GetItemTagCount(EntityGroupItem* item)
{
	return item ? item->tagCount : 0;
}

const char* EntityGroupDescriptor_GetItemTag(EntityGroupItem* item, size_t index)
{
	return (item && index < item->tagCount) ? item->tags[index] : NULL;
}

struct ResourcePoolTerrain* EntityGroupDescriptor_GetTerrainResource(EntityGroupItem* item)
{
	return item ? item->terrain : NULL;
}

struct ResourcePoolSpriteSheet* EntityGroupDescriptor_GetSpriteSheetResource(EntityGroupItem* item)
{
	return item ? item->spriteSheet : NULL;
}
