#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/gametypes.h"

struct OldEntity;

typedef struct OldPhysicsComponent
{
	struct OldEntity* ownerEntity;
	Vector2 velocity;
	Rectangle collisionHull;
	float gravityModifier;
	Mask32 collisionMask;
} OldPhysicsComponent;

OldPhysicsComponent PhysicsComponent_Create(struct OldEntity* ownerEntity);

Rectangle PhysicsComponent_GetWorldCollisionHull(const OldPhysicsComponent* component);
Vector2 PhysicsComponent_GetPosition(const OldPhysicsComponent* component);
