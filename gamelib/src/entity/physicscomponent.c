#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/entity.h"
#include "raymath.h"

PhysicsComponent PhysicsComponent_Create(struct Entity* ownerEntity)
{
	PhysicsComponent component = { 0 };

	component.ownerEntity = ownerEntity;
	component.gravityModifier = 1.0f;

	return component;
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
	return (component && component->ownerEntity) ? component->ownerEntity->position : Vector2Zero();
}
