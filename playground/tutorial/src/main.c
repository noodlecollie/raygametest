#include <stdio.h>
#include <float.h>
#include "gamelib/external/raylibheaders.h"
#include "raygui.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/entity/cameracomponent.h"
#include "gamelib/logic/playermovementlogic.h"
#include "gamelib/gameutil.h"
#include "gamelib/debugging/debugging.h"
#include "gamelib/stringutil.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	int screenWidth = 800;
	int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);

	InitWindow(screenWidth, screenHeight, "Tutorial");

	const Vector2 dpiScale = GetWindowScaleDPI();
	screenWidth = (int)((float)screenWidth * dpiScale.x);
	screenHeight = (int)((float)screenHeight * dpiScale.y);
	SetWindowSize(screenWidth, screenHeight);
	GuiSetStyle(DEFAULT, TEXT_SIZE, (int)((float)GuiGetStyle(DEFAULT, TEXT_SIZE) * dpiScale.x));

	World* world = World_Create();
	world->gravity = 1000.0f;

	Entity* terrainEnt = World_CreateEntity(world);
	TerrainComponent* terrain = Entity_CreateTerrainComponent(terrainEnt);
	terrain->scale = 12.0f;

	TerrainComponent_SetTerrain(terrain, "res/terrain/sample/sample.json");

	Entity* cameraEnt = World_CreateEntity(world);
	CameraComponent* camComp = Entity_CreateCameraComponent(cameraEnt);
	World_SetActiveCamera(world, camComp);

	const Vector2 playerStartPos = (Vector2){ 60.0f, 0.0f };

	Entity* playerEnt = World_CreateEntity(world);
	playerEnt->position = playerStartPos;

	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionMask = COLLISIONMASK_TERRAIN;
	playerPhys->collisionHull = (Rectangle){ -10.0f, -16.0f, 20.0f, 16.0f };
	playerPhys->enabled = true;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	SpriteComponent* playerSprite = Entity_CreateSpriteComponent(playerEnt);
	SpriteComponent_SetSpriteSheet(playerSprite, "res/sprites/raccoon/raccoon.json");

	LogicComponent* playerMovementLogic = Entity_AddLogicComponent(playerEnt);
	PlayerMovementLogic_SetOnComponent(playerMovementLogic);

	PlayerMovementLogicData* movementData = PlayerMovementLogic_GetDataFromComponent(playerMovementLogic);

	CopyString(movementData->animRunning, ARRAY_SIZE(movementData->animRunning), "run_right");
	CopyString(movementData->animStanding, ARRAY_SIZE(movementData->animStanding), "stand_right");
	CopyString(movementData->animJumping, ARRAY_SIZE(movementData->animJumping), "jump_right");
	CopyString(movementData->animFalling, ARRAY_SIZE(movementData->animFalling), "fall_right");

	movementData->fallSpeedAnimThreshold = 150.0f;

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		float wheelDelta = GetMouseWheelMove() * 0.25f;

		if ( wheelDelta >= 0.0f )
		{
			camComp->zoom *= 1.0f + wheelDelta;
		}
		else
		{
			camComp->zoom *= 1.0f / (1 - wheelDelta);
		}

		if ( camComp->zoom < 0.125f )
		{
			camComp->zoom = 0.125f;
		}
		else if ( camComp->zoom > 10.0f )
		{
			camComp->zoom = 10.0f;
		}

		if ( IsKeyPressed(KEY_R) )
		{
			playerEnt->position = playerStartPos;
			playerPhys->velocity = Vector2Zero();
			camComp->zoom = 1.0f;
		}

		World_Update(world);

		cameraEnt->position = playerEnt->position;

		BeginDrawing();

		ClearBackground(LIGHTGRAY);
		World_Render(world);

		EndDrawing();
	}

	World_Destroy(world);
	CloseWindow();

	return 0;
}
