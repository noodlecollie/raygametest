#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/oldentity/oldentity.h"

typedef struct Player
{
	OldEntity* entity;
	bool onGround;
} Player;

Player Player_Create(void);
void Player_Destroy(Player* player);
