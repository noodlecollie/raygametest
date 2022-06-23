#pragma once

#include "gamelib/entity/logiccomponent.h"
#include "gamelib/external/raylibheaders.h"

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

	// General movement
	Vector2 inputDir;	// Recalculated per frame

	// Zip movement
	float zipVelocityMultiplier;
	float zipDuration;
	double lastZipJumpTime;
} PlayerMovementLogicData;

void PlayerMovementLogic_SetOnComponent(LogicComponent* component);
PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component);
