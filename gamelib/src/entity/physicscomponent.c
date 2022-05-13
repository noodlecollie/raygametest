#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"

Rectangle OldPhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component)
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

Vector2 OldPhysicsComponent_GetPosition(const PhysicsComponent* component)
{
	return (component && component->ownerEntity) ? component->ownerEntity->position : Vector2Zero();
}
