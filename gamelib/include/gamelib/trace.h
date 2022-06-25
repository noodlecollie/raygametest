#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "gamelib/entity/componenttypes.h"
#include "gamelib/collisionclasses.h"

struct TerrainComponent;
struct Entity;

typedef struct TraceResult
{
	bool collided;
	bool beganColliding;
	bool endedColliding;	// If the trace ended while still inside a collider
	float fraction;
	Vector2 endPosition;
	Vector2 contactNormal;
	struct Entity* collisionEnt;
	ComponentType collisionComponentType;
} TraceResult;

TraceResult TraceResultNull(void);
TraceResult TraceResultNoCollision(Vector2 hullEndPos);

TraceResult TraceRectangleMovementAgainstTerrain(
	Rectangle hull,
	Vector2 delta,
	const struct TerrainComponent* terrain,
	CollisionMask collisionMask
);
