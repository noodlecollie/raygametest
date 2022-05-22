#include "gamelib/external/raylibheaders.h"
#include "gamelib/world.h"
#include "gamelib/physics.h"
#include "gamelib/entity/entity.h"
#include "entity/entityimpl.h"
#include "entity/cameracomponentimpl.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "listmacros.h"

typedef struct WorldImpl
{
	size_t entityCount;
	EntityImpl* entitiesHead;
	EntityImpl* entitiesTail;

	CameraComponent* activeCamera;
} WorldImpl;

static void DestroyAllEntities(WorldImpl* impl)
{
	EntityImpl* slot = impl->entitiesHead;

	impl->entitiesHead = NULL;
	impl->entitiesTail = NULL;
	impl->entityCount = 0;

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
	world->impl = (WorldImpl*)MemAlloc(sizeof(WorldImpl));

	world->gravity = 800.0f;

	return world;
}

void World_Destroy(World* world)
{
	if ( !world )
	{
		return;
	}

	DestroyAllEntities(world->impl);
	MemFree(world->impl);
	MemFree(world);
}

struct Entity* World_CreateEntity(World* world)
{
	if ( !world )
	{
		return NULL;
	}

	WorldImpl* impl = world->impl;

	if ( impl->entityCount >= WORLD_MAX_ENTITIES )
	{
		return NULL;
	}

	EntityImpl* slot = (EntityImpl*)MemAlloc(sizeof(EntityImpl));

	slot->ownerWorld = world;
	slot->entity.impl = slot;

	DBL_LL_ADD_TO_TAIL(slot, prev, next, impl, entitiesHead, entitiesTail);

	++impl->entityCount;

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
		slot->ownerWorld->impl->entitiesHead = slot->next;
	}

	if ( slot->next )
	{
		slot->next->prev = slot->prev;
	}
	else if ( slot->ownerWorld )
	{
		slot->ownerWorld->impl->entitiesTail = slot->prev;
	}

	if ( slot->ownerWorld && slot->ownerWorld->impl->entityCount > 0 )
	{
		--slot->ownerWorld->impl->entityCount;
	}

	EntityImpl_Destroy(slot);
}

struct Entity* World_GetEntityListHead(World* world)
{
	return (world && world->impl->entitiesHead) ? &world->impl->entitiesHead->entity : NULL;
}

struct Entity* World_GetPreviousEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	return (ent && ent->impl->prev) ? &ent->impl->prev->entity : NULL;
}

struct Entity* World_GetNextEntity(struct Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	return (ent && ent->impl->next) ? &ent->impl->next->entity : NULL;
}

size_t World_GetEntityCount(const World* world)
{
	return world ? world->impl->entityCount : 0;
}

void World_Think(World* world)
{
	if ( !world )
	{
		return;
	}

	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		for ( LogicComponent* logic = Entity_GetLogicComponentListHead(ent); logic; logic = Entity_GetNextLogicComponent(logic) )
		{
			if ( logic->callbacks.onPreThink )
			{
				logic->callbacks.onPreThink(logic);
			}
		}

		if ( Entity_GetPhysicsComponent(ent) )
		{
			Physics_SimulateObjectInWorld(world, Entity_GetPhysicsComponent(ent));
		}

		for ( LogicComponent* logic = Entity_GetLogicComponentListHead(ent); logic; logic = Entity_GetNextLogicComponent(logic) )
		{
			if ( logic->callbacks.onPostThink )
			{
				logic->callbacks.onPostThink(logic);
			}
		}
	}
}

void World_SetActiveCamera(World* world, struct CameraComponent* camera)
{
	if ( !world )
	{
		return;
	}

	world->impl->activeCamera = camera;
}

void World_Render(World* world)
{
	if ( !world )
	{
		return;
	}

	if ( !world->impl->activeCamera )
	{
		TraceLog(LOG_WARNING, "WORLD: Render call without active camera set!");
		return;
	}

	CameraComponentImpl* camImpl = world->impl->activeCamera->impl;
	Camera2D camera = { 0 };

	camera.target = camImpl->ownerEntity->position;
	camera.zoom = camImpl->component.zoom;

	const Vector2 windowDim = (Vector2){ (float)GetScreenWidth(), (float)GetScreenHeight() };
	camera.offset = Vector2Scale(windowDim, 0.5f);

	BeginMode2D(camera);

	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		EntityImpl_Render(ent->impl);
	}

	EndMode2D();
}
