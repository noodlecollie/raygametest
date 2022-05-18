#include <stdio.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gameutil.h"
#include "gamelib/world.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/logic/playerlogic.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	const int screenWidth = 800;
	const int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

	Camera2D camera = { 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	World* world = World_Create();
	world->gravity = 500.0f;

	Entity* terrainEnt = World_CreateEntity(world);
	TerrainComponent* terrain = Entity_CreateTerrainComponent(terrainEnt);
	terrain->scale = 10.0f;
	TerrainComponent_LoadLayer(terrain, 0, "res/maps/test.png");

	Entity* playerEnt = World_CreateEntity(world);
	PhysicsComponent* playerPhys = Entity_CreatePhysicsComponent(playerEnt);
	playerPhys->collisionHull = (Rectangle){ -5.0f, -5.0f, 10.0f, 10.0f };
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->enabled = true;
	playerPhys->movementType = PHYSMOVE_SLIDE;

	LogicComponent* playerLogic = Entity_AddLogicComponent(playerEnt);
	PlayerLogic_SetOnComponent(playerLogic);

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		camera.zoom += (float)GetMouseWheelMove() * 0.05f;

		if ( camera.zoom > 3.0f )
		{
			camera.zoom = 3.0f;
		}
		else if ( camera.zoom < 0.25f )
		{
			camera.zoom = 0.25f;
		}

		if ( IsKeyPressed(KEY_R) )
		{
			camera.zoom = 1.0f;
		}

		World_Think(world);

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
		}

		Rectangle playerRect = PhysicsComponent_GetWorldCollisionHull(playerPhys);
		DrawRectangle((int)playerRect.x, (int)playerRect.y, (int)playerRect.width, (int)playerRect.height, RED);

		EndMode2D();

		char buffer[64];
		snprintf(buffer, sizeof(buffer), "Player: (%.2f, %.2f) [%.2f, %.2f]", playerEnt->position.x, playerEnt->position.y, playerPhys->velocity.x, playerPhys->velocity.y);
		DrawText(buffer, 10, 10, 10, BLACK);

		EndDrawing();
	}

	World_Destroy(world);
	CloseWindow();

	return 0;
}
