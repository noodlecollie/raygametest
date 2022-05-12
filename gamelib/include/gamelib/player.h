#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "gamelib/entity/entity.h"

// TODO: This should be a component on an entity, instead of an entity within a player
typedef struct Player
{
	Entity entity;
	bool onGround;
} Player;

Player Player_Create(void);
void Player_Destroy(Player* player);
