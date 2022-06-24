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

	// Movement speed
	float horizontalInputSpeed;
	float jumpImpulse;

	// General movement, recalculated per frame
	Vector2 inputDir;
	Vector2 wishVel;

	// Zip movement
	float zipVelocityMultiplier;
	float zipJumpImpulse;
	float zipDuration;
	bool activatedZipJump;
	double lastZipJumpTime;
} PlayerMovementLogicData;

void PlayerMovementLogic_SetOnComponent(LogicComponent* component);
PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component);
