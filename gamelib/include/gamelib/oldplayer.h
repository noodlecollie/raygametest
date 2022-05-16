#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/oldentity/oldentity.h"

typedef struct OldPlayer
{
	OldEntity* entity;
	bool onGround;
} OldPlayer;

OldPlayer OldPlayer_Create(void);
void OldPlayer_Destroy(OldPlayer* player);
