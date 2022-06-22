#pragma once

#include "gamelib/entity/logiccomponent.h"

#define PLAYERLOGIC_ANIM_NAME_MAX_LENGTH 32

typedef struct PlayerLogicData
{
	bool onGround;

	// Animations
	char animStanding[PLAYERLOGIC_ANIM_NAME_MAX_LENGTH];
	char animRunning[PLAYERLOGIC_ANIM_NAME_MAX_LENGTH];
	char animJumping[PLAYERLOGIC_ANIM_NAME_MAX_LENGTH];
	char animFalling[PLAYERLOGIC_ANIM_NAME_MAX_LENGTH];
	float fallSpeedAnimThreshold;

	// Zip movement
	float velocityMultiplier;
	double lastJumpTime;
} PlayerMovementLogicData;

void PlayerMovementLogic_SetOnComponent(LogicComponent* component);
PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component);
