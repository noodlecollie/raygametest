#include <stdlib.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/oldentity/oldentity.h"
#include "gamelib/oldentity/oldcomponenttypes.h"
#include "gamelib/gameutil.h"

typedef struct OldEntityComponentList
{
	void* list[OLDCOMPONENT_TOTAL_TYPES];
} OldEntityComponentList;

static inline void FreeComponent(OldEntity* ent, OldComponentType componentType)
{
	if ( !ent || !ent->components->list[componentType] )
	{
		return;
	}

	MemFree(ent->components->list[componentType]);
	ent->components->list[componentType] = NULL;
}

OldEntity* OldEntity_Create(void)
{
	OldEntity* ent = MemAlloc(sizeof(struct OldEntity));

	if ( ent )
	{
		ent->components = MemAlloc(sizeof(OldEntityComponentList));

		if ( !ent->components )
		{
			MemFree(ent);
			ent = NULL;
		}
	}

	return ent;
}

void OldEntity_Destroy(OldEntity* ent)
{
	if ( ent )
	{
		for ( size_t index = 0; index < OLDCOMPONENT_TOTAL_TYPES; ++index )
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

OldPhysicsComponent* OldEntity_PhysicsComponent(OldEntity* ent)
{
	return ent ? (OldPhysicsComponent*)ent->components->list[OLDCOMPONENT_PHYSICS] : NULL;
}

OldPhysicsComponent* OldEntity_AddPhysicsComponent(OldEntity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	OldPhysicsComponent** component = (OldPhysicsComponent**)&ent->components->list[OLDCOMPONENT_PHYSICS];

	if ( !(*component) )
	{
		*component = MemAlloc(sizeof(OldPhysicsComponent));

		if ( *component )
		{
			**component = OldPhysicsComponent_Create(ent);
		}
	}

	return *component;
}

void OldEntity_RemovePhysicsComponent(OldEntity* ent)
{
	FreeComponent(ent, OLDCOMPONENT_PHYSICS);
}
