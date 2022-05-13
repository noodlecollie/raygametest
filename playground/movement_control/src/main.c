#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "gamelib/oldworld.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "gamelib/player.h"
#include "gamelib/platformmovement.h"

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

	OldWorld world = { 0 };
	world.gravity = 1000.0f;
	world.level.scale = 10.0f;
	Terrain_LoadLayer(&world.level, 0, "res/maps/test.png");
	Vector2i levelDim = Terrain_GetLayerDimensions(world.level, 0);

	Camera2D camera = { 0 };
	camera.target = (Vector2){ ((float)levelDim.x / 2.0f) * world.level.scale, ((float)levelDim.y / 2.0f) * world.level.scale };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	Player player = Player_Create();

	OldPhysicsComponent* playerPhys = OldEntity_AddPhysicsComponent(player.entity);
	playerPhys->collisionMask = 0xFFFFFFFF;
	playerPhys->gravityModifier = 1.0f;
	playerPhys->ownerEntity->position = (Vector2){ 1000.0f, 0.0f };
	playerPhys->collisionHull = (Rectangle){ -5.0f, -10.0f, 10.0f, 20.0f };

	const Vector2 movementScale = (Vector2){ 200.0f, 450.0f };

	Vector2 startPos = playerPhys->ownerEntity->position;

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
			playerPhys->ownerEntity->position = startPos;
			playerPhys->velocity = Vector2Zero();
			camera.zoom = 1.0f;
		}

		camera.target = playerPhys->ownerEntity->position;

		Vector2 movement = Vector2Zero();

		if ( IsKeyDown(KEY_RIGHT) )
		{
			movement.x += 1.0f;
		}

		if ( IsKeyDown(KEY_LEFT) )
		{
			movement.x -= 1.0f;
		}

		if ( IsKeyPressed(KEY_UP) && player.onGround )
		{
			movement.y -= 1.0f;
		}

		movement = Vector2Multiply(movement, movementScale);

		playerPhys->velocity.x = movement.x;
		playerPhys->velocity.y += movement.y;

		PlatformMovement_MovePlayer(&player, &world);

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		Vector2i dims = Terrain_GetLayerDimensions(world.level, 0);

		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = Terrain_GetBlockWorldRectByCoOrds(world.level, (Vector2i){ x, y });
				Color blockColour = Terrain_GetBlockColourByCoOrds(world.level, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);
			}

			DrawRectangleRec(OldPhysicsComponent_GetWorldCollisionHull(playerPhys), player.onGround ? YELLOW : RED);
		}

		EndMode2D();

		int leftMargin = (int)(10.0f * dpiScale.x);
		int fontSize = (int)(10.0f * dpiScale.y);

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Player position: (%.2f, %.2f)", playerPhys->ownerEntity->position.x, playerPhys->ownerEntity->position.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(10.0f * dpiScale.y), fontSize, BLUE);

		snprintf(buffer, sizeof(buffer), "Player velocity: (%.2f, %.2f)", playerPhys->velocity.x, playerPhys->velocity.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(30.0f * dpiScale.y), fontSize, BLUE);

		EndDrawing();
	}

	Player_Destroy(&player);
	Terrain_Unload(world.level);
	CloseWindow();

	return 0;
}
