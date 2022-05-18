#include <stdio.h>
#include "gamelib/external/raylibheaders.h"
#include "raygui.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/logic/playerlogic.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	int screenWidth = 800;
	int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Movement Control");

	const Vector2 dpiScale = GetWindowScaleDPI();
	screenWidth = (int)((float)screenWidth * dpiScale.x);
	screenHeight = (int)((float)screenHeight * dpiScale.y);
	SetWindowSize(screenWidth, screenHeight);
	GuiSetStyle(DEFAULT, TEXT_SIZE, (int)((float)GuiGetStyle(DEFAULT, TEXT_SIZE) * dpiScale.x));

	World* world = World_Create();
	world->gravity = 1000.0f;

	Entity* terrainEnt = World_CreateEntity(world);
	TerrainComponent* terrain = Entity_CreateTerrainComponent(terrainEnt);
	terrain->scale = 10.0f;

	TerrainComponent_LoadLayer(terrain, 0, "res/maps/test.png");
	Vector2i levelDim = TerrainComponent_GetLayerDimensions(terrain, 0);

	Camera2D camera = { 0 };
	camera.target = (Vector2){ ((float)levelDim.x / 2.0f) * terrain->scale, ((float)levelDim.y / 2.0f) * terrain->scale };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	const Vector2 playerStartPos = (Vector2){ 1000.0f, 0.0f };

	Entity* playerEnt = World_CreateEntity(world);
	playerEnt->position = playerStartPos;

	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->collisionHull = (Rectangle){ -5.0f, -10.0f, 10.0f, 20.0f };
	playerPhys->enabled = true;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	LogicComponent* playerLogic = Entity_AddLogicComponent(playerEnt);
	PlayerLogic_SetOnComponent(playerLogic);

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		float wheelDelta = GetMouseWheelMove() * 0.25f;

		if ( wheelDelta >= 0.0f )
		{
			camera.zoom *= 1.0f + wheelDelta;
		}
		else
		{
			camera.zoom *= 1.0f / (1 - wheelDelta);
		}

		if ( camera.zoom < 0.125f )
		{
			camera.zoom = 0.125f;
		}
		else if ( camera.zoom > 10.0f )
		{
			camera.zoom = 10.0f;
		}

		if ( IsKeyPressed(KEY_R) )
		{
			playerEnt->position = playerStartPos;
			playerPhys->velocity = Vector2Zero();
			camera.zoom = 1.0f;
		}

		World_Think(world);

		camera.target = playerEnt->position;

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		Vector2i dims = TerrainComponent_GetLayerDimensions(terrain, 0);

		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = TerrainComponent_GetBlockWorldRectByCoOrds(terrain, (Vector2i){ x, y });
				Color blockColour = TerrainComponent_GetBlockColourByCoOrds(terrain, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);
			}

			DrawRectangleRec(PhysicsComponent_GetWorldCollisionHull(playerPhys), PlayerLogic_GetDataFromComponent(playerLogic)->onGround ? YELLOW : RED);
		}

		EndMode2D();

		int leftMargin = (int)(10.0f * dpiScale.x);
		int fontSize = (int)(10.0f * dpiScale.y);

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Player position: (%.2f, %.2f)", playerEnt->position.x, playerEnt->position.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(10.0f * dpiScale.y), fontSize, BLUE);

		snprintf(buffer, sizeof(buffer), "Player velocity: (%.2f, %.2f)", playerPhys->velocity.x, playerPhys->velocity.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(30.0f * dpiScale.y), fontSize, BLUE);

		EndDrawing();
	}

	World_Destroy(world);
	CloseWindow();

	return 0;
}
