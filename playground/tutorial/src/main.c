#include <stdio.h>
#include "gamelib/external/raylibheaders.h"
#include "raygui.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/entity/cameracomponent.h"
#include "gamelib/logic/playerlogic.h"
#include "gamelib/gameutil.h"

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

	TerrainComponent_LoadLayer(terrain, 0, "res/terrain/terrain.png");

	Entity* cameraEnt = World_CreateEntity(world);
	CameraComponent* camComp = Entity_CreateCameraComponent(cameraEnt);
	World_SetActiveCamera(world, camComp);

	const Vector2 playerStartPos = (Vector2){ 0.0f, 0.0f };

	Entity* playerEnt = World_CreateEntity(world);
	playerEnt->position = playerStartPos;

	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->collisionHull = (Rectangle){ -5.0f, -10.0f, 10.0f, 20.0f };
	playerPhys->enabled = true;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	SpriteComponent* playerSprite = Entity_CreateSpriteComponent(playerEnt);
	SpriteComponent_SetSpriteSheet(playerSprite, "res/sprites/raccoon/raccoon.json");
	playerSprite->offset.x = -16;
	playerSprite->offset.y = -22;

	LogicComponent* playerLogic = Entity_AddLogicComponent(playerEnt);
	PlayerLogic_SetOnComponent(playerLogic);

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