#include "gamelib/entity/entity.h"
#include "entity/entityimpl.h"

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* entity)
{
	return entity ? EntityImpl_GetPhysicsComponent(entity->impl) : NULL;
}

struct PhysicsComponent* Entity_AddPhysicsComponent(Entity* entity)
{
	return entity ? EntityImpl_AddPhysicsComponent(entity->impl) : NULL;
}

void Entity_RemovePhysicsComponent(Entity* entity)
{
	if ( entity )
	{
		EntityImpl_RemovePhysicsComponent(entity->impl);
	}
}
