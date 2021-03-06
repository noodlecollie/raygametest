#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "gamelib/collisionclasses.h"

struct Entity;
struct PhysicsComponentImpl;

typedef enum PhysicsMovementType
{
	PHYSMOVE_NONE = 0,
	PHYSMOVE_STICK,
	PHYSMOVE_SLIDE
} PhysicsMovementType;

typedef struct PhysicsComponent
{
	struct PhysicsComponentImpl* impl;

	bool enabled;
	Vector2 velocity;
	Rectangle collisionHull;
	float gravityModifier;
	CollisionMask collisionMask;
	PhysicsMovementType movementType;
} PhysicsComponent;

struct Entity* PhysicsComponent_GetOwnerEntity(const PhysicsComponent* component);
Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component);
Vector2 PhysicsComponent_GetPosition(const PhysicsComponent* component);
