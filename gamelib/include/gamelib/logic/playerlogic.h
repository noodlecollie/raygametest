#pragma once

#include "gamelib/entity/logiccomponent.h"

typedef struct PlayerLogicData
{
	bool onGround;
} PlayerLogicData;

void PlayerLogic_SetOnComponent(LogicComponent* component);
PlayerLogicData* PlayerLogic_GetDataFromComponent(LogicComponent* component);
