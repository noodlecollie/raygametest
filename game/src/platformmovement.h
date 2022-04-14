#pragma once

#include <stdint.h>
#include "platformerlevel.h"
#include "player.h"

void PlatformMovement_MovePlayer(Player* player, float deltaTime, PlatformerLevel level, uint32_t collisionLayers);
