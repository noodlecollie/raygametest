#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/gametypes.h"

struct Entity;

typedef struct PhysicsComponent
{
	struct Entity* ownerEntity;
	Vector2 velocity;
	Rectangle collisionHull;
	float gravityModifier;
	Mask32 collisionMask;
} PhysicsComponent;

Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component);
Vector2 PhysicsComponent_GetPosition(const PhysicsComponent* component);
