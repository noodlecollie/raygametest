#include <stdlib.h>
#include "raylib.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/componenttypes.h"

struct Entity
{
	void* components[COMPONENT_TOTAL_TYPES];
};

static inline void FreeComponent(Entity ent, ComponentType componentType)
{
	if ( !ent || !ent->components[componentType] )
	{
		return;
	}

	MemFree(ent->components[componentType]);
	ent->components[componentType] = NULL;
}

Entity Entity_Create(void)
{
	Entity ent;
	ent = MemAlloc(sizeof(struct Entity));

	return ent;
}

void Entity_Destroy(Entity ent)
{
	if ( ent )
	{
		for ( size_t index = 0; index < COMPONENT_TOTAL_TYPES; ++index )
		{
			if ( ent->components[index] )
			{
				MemFree(ent->components[index]);
			}
		}

		MemFree(ent);
	}
}

PhysicsComponent* Entity_PhysicsComponent(Entity ent)
{
	if ( !ent )
	{
		return NULL;
	}

	PhysicsComponent** component = (PhysicsComponent**)&ent->components[COMPONENT_PHYSICS];

	if ( !(*component) )
	{
		*component = MemAlloc(sizeof(PhysicsComponent));
		**component = PhysicsComponent_Create();
	}

	return *component;
}

void Entity_RemovePhysicsComponent(Entity ent)
{
	FreeComponent(ent, COMPONENT_PHYSICS);
}
