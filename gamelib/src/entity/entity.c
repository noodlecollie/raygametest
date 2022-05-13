#include <stdlib.h>
#include "raylib.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/componenttypes.h"
#include "gamelib/gameutil.h"

typedef struct EntityComponentList
{
	void* list[COMPONENT_TOTAL_TYPES];
} EntityComponentList;

static inline void FreeComponent(Entity* ent, ComponentType componentType)
{
	if ( !ent || !ent->components->list[componentType] )
	{
		return;
	}

	MemFree(ent->components->list[componentType]);
	ent->components->list[componentType] = NULL;
}

Entity* Entity_Create(void)
{
	Entity* ent = MemAlloc(sizeof(struct Entity));

	if ( ent )
	{
		ent->components = MemAlloc(sizeof(EntityComponentList));

		if ( !ent->components )
		{
			MemFree(ent);
			ent = NULL;
		}
	}

	return ent;
}

void Entity_Destroy(Entity* ent)
{
	if ( ent )
	{
		for ( size_t index = 0; index < COMPONENT_TOTAL_TYPES; ++index )
		{
			if ( ent->components->list[index] )
			{
				MemFree(ent->components->list[index]);
			}
		}

		MemFree(ent->components);
		MemFree(ent);
	}
}

PhysicsComponent* Entity_PhysicsComponent(Entity* ent)
{
	return ent ? (PhysicsComponent*)ent->components->list[COMPONENT_PHYSICS] : NULL;
}

PhysicsComponent* Entity_AddPhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	PhysicsComponent** component = (PhysicsComponent**)&ent->components->list[COMPONENT_PHYSICS];

	if ( !(*component) )
	{
		*component = MemAlloc(sizeof(PhysicsComponent));

		if ( *component )
		{
			**component = PhysicsComponent_Create(ent);
		}
	}

	return *component;
}

void Entity_RemovePhysicsComponent(Entity* ent)
{
	FreeComponent(ent, COMPONENT_PHYSICS);
}
