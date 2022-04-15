#pragma once

#include <stdint.h>
#include "gamelib/platformerlevel.h"
#include "gamelib/player.h"

void PlatformMovement_MovePlayer(Player* player, float deltaTime, PlatformerLevel level, uint32_t collisionLayers);
