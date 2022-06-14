#pragma once

#include "gamelib/entity/logiccomponent.h"

typedef struct PlayerLogicData
{
	bool onGround;
} PlayerMovementLogicData;

void PlayerMovementLogic_SetOnComponent(LogicComponent* component);
PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component);
