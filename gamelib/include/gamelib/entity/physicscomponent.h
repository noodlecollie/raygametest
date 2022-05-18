#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"

struct Entity;
struct PhysicsComponentImpl;

typedef struct PhysicsComponent
{
	struct PhysicsComponentImpl* impl;

	bool enabled;
	Vector2 velocity;
	Rectangle collisionHull;
	float gravityModifier;
	Mask32 collisionMask;
} PhysicsComponent;

struct Entity* PhysicsComponent_GetOwnerEntity(const PhysicsComponent* component);
Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component);
Vector2 PhysicsComponent_GetPosition(const PhysicsComponent* component);
