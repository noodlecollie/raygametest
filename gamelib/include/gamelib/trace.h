#pragma once

#include <stdint.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"
#include "gamelib/terrain.h"
#include "gamelib/oldplayer.h"

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
} TraceResult;

TraceResult TraceResultNull(void);
TraceResult TraceResultNoCollision(Vector2 hullEndPos);

TraceResult TraceRectangleMovementInLevel(Rectangle hull, Vector2 delta, Terrain level, Mask32 collisionLayers);
TraceResult TraceRectangleMovementAgainstTerrain(Rectangle hull, Vector2 delta, struct TerrainComponent* terrain, Mask32 collisionLayers);
