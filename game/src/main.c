#include <stdio.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/entity/cameracomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/logic/playermovementlogic.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	const int screenWidth = 800;
	const int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Game");

	World* world = World_Create();
	world->gravity = 500.0f;

	Entity* terrainEnt = World_CreateEntity(world);
	TerrainComponent* terrain = Entity_CreateTerrainComponent(terrainEnt);
	terrain->scale = 30.0f;
	TerrainComponent_SetTerrain(terrain, "res/terrain/sample.json");

	Entity* playerEnt = World_CreateEntity(world);

	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionHull = (Rectangle){ -5.0f, -5.0f, 10.0f, 10.0f };
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->enabled = false;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	SpriteComponent* playerSprite = Entity_CreateSpriteComponent(playerEnt);
	SpriteComponent_SetSpriteSheet(playerSprite, "res/sprites/runcycle/runcycle.json");
	playerSprite->scale = (Vector2){ 0.5f, 0.5f };

	LogicComponent* playerMovementLogic = Entity_AddLogicComponent(playerEnt);
	PlayerMovementLogic_SetOnComponent(playerMovementLogic);

	Entity* cameraEnt = World_CreateEntity(world);
	CameraComponent* camComp = Entity_CreateCameraComponent(cameraEnt);
	camComp->zoom = 1.0f;

	World_SetActiveCamera(world, camComp);

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		camComp->zoom += (float)GetMouseWheelMove() * 0.05f;

		if ( camComp->zoom > 3.0f )
		{
			camComp->zoom = 3.0f;
		}
		else if ( camComp->zoom < 0.25f )
		{
			camComp->zoom = 0.25f;
		}

		if ( IsKeyPressed(KEY_R) )
		{
			camComp->zoom = 1.0f;
		}

		World_Update(world);

		BeginDrawing();
		ClearBackground(LIGHTGRAY);
		World_Render(world);
		EndDrawing();
	}

	World_Destroy(world);
	CloseWindow();

	return 0;
}
