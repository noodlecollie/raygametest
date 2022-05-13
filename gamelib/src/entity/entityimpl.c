#include "entity/entityimpl.h"
#include "gamelib/gameutil.h"

static void DestroyAllComponents(EntityImpl* impl)
{
	for ( size_t index = 0; index < ARRAY_SIZE(impl->components); ++index )
	{
		if ( impl->components[index] )
		{
			MemFree(impl->components[index]);
			impl->components[index] = NULL;
		}
	}
}

void EntityImpl_Destroy(EntityImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	DestroyAllComponents(impl);
	MemFree(impl);
}

struct PhysicsComponent* EntityImpl_GetPhysicsComponent(Entity* ent)
{
	return (ent && ent->impl) ? ent->impl->components[COMPONENT_PHYSICS] : NULL;
}

struct PhysicsComponent* EntityImpl_AddPhysicsComponent(Entity* ent)
{
	if ( !ent || !ent->impl )
	{
		return NULL;
	}

	PhysicsComponent** component = (PhysicsComponent**)&ent->impl->components[COMPONENT_PHYSICS];

	if ( !*component )
	{
		*component = (PhysicsComponent*)MemAlloc(sizeof(PhysicsComponent));

		(*component)->ownerEntity = &ent->impl->entity;
		(*component)->gravityModifier = 1.0f;
	}

	return *component;
}

void EntityImpl_RemovePhysicsComponent(Entity* ent)
{
	if ( !ent || !ent->impl )
	{
		return;
	}

	PhysicsComponent** component = (PhysicsComponent**)&ent->impl->components[COMPONENT_PHYSICS];

	if ( *component )
	{
		MemFree(*component);
		*component = NULL;
	}
}
