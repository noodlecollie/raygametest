#include "entity/physicscomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"

PhysicsComponentImpl* PhysicsComponentImpl_Create(struct Entity* ownerEntity)
{
	if ( !ownerEntity )
	{
		return NULL;
	}

	PhysicsComponentImpl* impl = (PhysicsComponentImpl*)MemAlloc(sizeof(PhysicsComponentImpl));

	impl->component.impl = impl;
	impl->ownerEntity = ownerEntity;
	impl->component.gravityModifier = 1.0f;

	return impl;
}

void PhysicsComponentImpl_Destroy(PhysicsComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	MemFree(impl);
}

struct Entity* PhysicsComponent_GetOwnerEntity(const PhysicsComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}

Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component)
{
	Rectangle hull = { 0, 0, 0, 0 };

	if ( !component )
	{
		return hull;
	}

	Vector2 pos = PhysicsComponent_GetPosition(component);

	hull.x = pos.x + component->collisionHull.x;
	hull.y = pos.y + component->collisionHull.y;
	hull.width = component->collisionHull.width;
	hull.height = component->collisionHull.height;

	return hull;
}

Vector2 PhysicsComponent_GetPosition(const PhysicsComponent* component)
{
	return (component && component->impl->ownerEntity) ? component->impl->ownerEntity->position : Vector2Zero();
}
