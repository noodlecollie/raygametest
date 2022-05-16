#pragma once

#include "gamelib/entity/logiccomponent.h"

typedef struct PlayerLogic
{
	bool onGround;
} PlayerLogic;

void PlayerLogic_InitComponent(LogicComponent* component);
