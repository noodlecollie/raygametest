#include "gamelib/world.h"
#include "gamelib/oldentity/oldentity.h"
#include "raylib.h"

typedef struct EntitySlot
{
	struct EntitySlot* next;
	struct EntitySlot* prev;
	OldEntity* entity;
} EntitySlot;

struct World
{
	size_t entityCount;
	EntitySlot* entities;
};

static void DestroyAllEntities(World* world)
{
	EntitySlot* slot = world->entities;

	world->entities = NULL;
	world->entityCount = 0;

	while ( slot )
	{
		EntitySlot* next = slot->next;

		OldEntity_Destroy(slot->entity);
		MemFree(slot);

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
