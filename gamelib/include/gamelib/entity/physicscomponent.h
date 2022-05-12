#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/gametypes.h"

typedef struct PhysicsComponent
{
	Vector2 position;
	Vector2 velocity;
	Rectangle collisionHull;
	float gravityModifier;
	Mask32 collisionMask;
} PhysicsComponent;

PhysicsComponent PhysicsComponent_Create(void);

Rectangle PhysicsComponent_GetWorldCollisionHull(const PhysicsComponent* component);
