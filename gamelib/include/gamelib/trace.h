#pragma once

#include <stdint.h>
#include "raylib.h"
#include "gamelib/platformerlevel.h"
#include "gamelib/player.h"

typedef struct TraceResult
{
	bool collided;
	bool beganColliding;
	bool endedColliding;
	float fraction;
	Vector2 contactPosition;
	Vector2 contactNormal;
} TraceResult;

TraceResult TraceRectangleMovementInLevel(Rectangle hull, Vector2 delta, PlatformerLevel level, uint32_t collisionLayers);
