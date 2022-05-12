#include "gamelib/entity/physicscomponent.h"

Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component)
{
	Rectangle hull = { 0, 0, 0, 0 };

	if ( !component )
	{
		return hull;
	}

	hull.x = component->position.x + component->collisionHull.x;
	hull.y = component->position.y + component->collisionHull.y;
	hull.width = component->collisionHull.width;
	hull.height = component->collisionHull.height;

	return hull;
}

PhysicsComponent PhysicsComponent_Create(void)
{
	PhysicsComponent component = { 0 };

	component.gravityModifier = 1.0f;

	return component;
}
