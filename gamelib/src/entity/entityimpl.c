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

PhysicsComponent* EntityImpl_GetPhysicsComponent(EntityImpl* impl)
{
	return impl ? impl->components[COMPONENT_PHYSICS] : NULL;
}

PhysicsComponent* EntityImpl_AddPhysicsComponent(EntityImpl* impl)
{
	if ( !impl )
	{
		return NULL;
	}

	PhysicsComponent** component = (PhysicsComponent**)&impl->components[COMPONENT_PHYSICS];

	if ( !*component )
	{
		*component = (PhysicsComponent*)MemAlloc(sizeof(PhysicsComponent));

		(*component)->ownerEntity = &impl->entity;
		(*component)->gravityModifier = 1.0f;
	}

	return *component;
}

void EntityImpl_RemovePhysicsComponent(EntityImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	PhysicsComponent** component = (PhysicsComponent**)&impl->components[COMPONENT_PHYSICS];

	if ( *component )
	{
		MemFree(*component);
		*component = NULL;
	}
}
