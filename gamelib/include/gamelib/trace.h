#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/gametypes.h"
#include "gamelib/terrain.h"
#include "gamelib/player.h"

typedef struct TraceResult
{
	bool collided;
	bool beganColliding;
	bool endedColliding;	// If the trace ended while still inside a collider
	float fraction;
	Vector2 endPosition;
	Vector2 contactNormal;
} TraceResult;

TraceResult TraceRectangleMovementInLevel(Rectangle hull, Vector2 delta, Terrain level, Mask32 collisionLayers);
