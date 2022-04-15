#pragma once

#include <stdint.h>
#include "gamelib/platformerlevel.h"
#include "gamelib/player.h"

typedef struct MovementResult
{
	bool collided;
	Vector2 contactPosition;
	Vector2 contactNormal;
} MovementData;

void PlatformMovement_MovePlayer(Player* player, float deltaTime, PlatformerLevel level, uint32_t collisionLayers);
