#pragma once

#include <stdint.h>
#include "gamelib/world.h"
#include "gamelib/player.h"

void PlatformMovement_MovePlayer(Player* player, float deltaTime, World* world, uint32_t collisionLayers);
