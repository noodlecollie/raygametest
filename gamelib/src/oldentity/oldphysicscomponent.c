#include "gamelib/oldentity/oldphysicscomponent.h"
#include "gamelib/oldentity/oldentity.h"
#include "raymath.h"

OldPhysicsComponent OldPhysicsComponent_Create(struct OldEntity* ownerEntity)
{
	OldPhysicsComponent component = { 0 };

	component.ownerEntity = ownerEntity;
	component.gravityModifier = 1.0f;

	return component;
}

Rectangle OldPhysicsComponent_GetWorldCollisionHull(const OldPhysicsComponent* component)
{
	Rectangle hull = { 0, 0, 0, 0 };

	if ( !component )
	{
		return hull;
	}

	Vector2 pos = OldPhysicsComponent_GetPosition(component);

	hull.x = pos.x + component->collisionHull.x;
	hull.y = pos.y + component->collisionHull.y;
	hull.width = component->collisionHull.width;
	hull.height = component->collisionHull.height;

	return hull;
}

Vector2 OldPhysicsComponent_GetPosition(const OldPhysicsComponent* component)
{
	return (component && component->ownerEntity) ? component->ownerEntity->position : Vector2Zero();
}
