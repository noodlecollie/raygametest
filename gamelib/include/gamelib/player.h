#pragma once

#include <stdbool.h>
#include "raylib.h"
#include "gamelib/entity/entity.h"

typedef struct Player
{
	Entity* entity;
	bool onGround;
} Player;

Player Player_Create(void);
void Player_Destroy(Player* player);
