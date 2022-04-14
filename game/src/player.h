#pragma once

#include "raylib.h"

typedef struct Player
{
	Vector2 position;

	// Units/sec
	Vector2 velocity;

	Rectangle collisionHull;
} Player;

Rectangle Player_GetWorldCollisionHull(const Player* player);
