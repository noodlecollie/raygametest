#include "asyncload/asyncload.h"
#include "entity/entitygroup.h"
#include "threading.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/stringutil.h"
#include "descriptor/entitygroupdescriptor.h"
#include "gamelib/entity/entity.h"

AsyncLoadArgs* AsyncLoadArgs_Create(sem_t* semaphore, const char* filePath)
{
	AsyncLoadArgs* args = (AsyncLoadArgs*)MemAlloc(sizeof(AsyncLoadArgs));

	args->semaphore = semaphore;
	args->filePath = DuplicateString(filePath);

	return args;
}

void AsyncLoadArgs_Destroy(AsyncLoadArgs* args)
{
	if ( !args )
	{
		return;
	}

	if ( args->loadedEntityGroup )
	{
		EntityGroup_Destroy(args->loadedEntityGroup);
		args->loadedEntityGroup = NULL;
	}

	if ( args->filePath )
	{
		MemFree(args->filePath);
		args->filePath = NULL;
	}

	MemFree(args);
}

static void PopulateEntityComponents(Entity* ent, EntityGroupItem* groupItem)
{
	// TODO
	(void)ent;
	(void)groupItem;
}

static EntityGroup* LoadEntityGroupDescriptor(const char* filePath)
{
	EntityGroupDescriptor* descriptor = EntityGroupDescriptor_LoadFromJSONFile(filePath);

	if ( !descriptor || EntityGroupDescriptor_GetItemCount(descriptor) < 1 )
	{
		return NULL;
	}

	EntityGroup* group = EntityGroup_Create(NULL);

	for ( EntityGroupItem* item = EntityGroupDescriptor_GetFirstItem(descriptor);
	      item;
	      item = EntityGroupDescriptor_GetNextItem(item) )
	{
		// TODO: Create an entity in the group and assign it stuff.
		struct Entity* ent = EntityGroup_CreateEntity(group);
		PopulateEntityComponents(ent, item);
	}

	EntityGroupDescriptor_Destroy(descriptor);
	return group;
}

void* AsyncLoad_Routine(void* args)
{
	AsyncLoadArgs* inArgs = (AsyncLoadArgs*)args;

	if ( inArgs->filePath && inArgs->filePath[0] )
	{
		inArgs->loadedEntityGroup = LoadEntityGroupDescriptor(inArgs->filePath);
	}

	THREADING_DECREMENT_SEMAPHORE(inArgs->semaphore);
	return NULL;
}
