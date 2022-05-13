#include "gamelib/external/raylibheaders.h"
#include "gamelib/world.h"
#include "entity/entityimpl.h"

struct World
{
	size_t entityCount;
	EntityImpl* entitiesHead;
	EntityImpl* entitiesTail;
};

static void DestroyAllEntities(World* world)
{
	EntityImpl* slot = world->entitiesHead;

	world->entitiesHead = NULL;
	world->entitiesTail = NULL;
	world->entityCount = 0;

	while ( slot )
	{
		EntityImpl* next = slot->next;
		EntityImpl_Destroy(slot);
		slot = next;
	}
}

World* World_Create(void)
{
	World* world = (World*)MemAlloc(sizeof(World));

	return world;
}

void World_Destroy(World* world)
{
	if ( !world )
	{
		return;
	}

	DestroyAllEntities(world);
	MemFree(world);
}

struct Entity* World_CreateEntity(World* world)
{
	if ( !world || world->entityCount >= WORLD_MAX_ENTITIES )
	{
		return NULL;
	}

	EntityImpl* slot = (EntityImpl*)MemAlloc(sizeof(EntityImpl));

	if ( !slot )
	{
		return NULL;
	}

	slot->ownerWorld = world;
	slot->prev = world->entitiesTail;

	if ( world->entitiesTail )
	{
		world->entitiesTail->next = slot;
	}

	slot->entity.impl = slot;

	return &slot->entity;
}

void World_DestroyEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	EntityImpl* slot = ent->impl;

	if ( slot->prev )
	{
		slot->prev->next = slot->next;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->entitiesHead = slot->next;
	}

	if ( slot->next )
	{
		slot->next->prev = slot->prev;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->entitiesTail = slot->prev;
	}

	if ( slot->ownerWorld && slot->ownerWorld->entityCount > 0 )
	{
		--slot->ownerWorld->entityCount;
	}

	EntityImpl_Destroy(slot);
}
