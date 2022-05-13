#pragma once

#include <stdint.h>
#include "gamelib/external/raylibheaders.h"
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

OldPhysicsComponent OldPhysicsComponent_Create(struct OldEntity* ownerEntity);

Rectangle OldPhysicsComponent_GetWorldCollisionHull(const OldPhysicsComponent* component);
Vector2 OldPhysicsComponent_GetPosition(const OldPhysicsComponent* component);
