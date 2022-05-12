#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "gamelib/world.h"
#include "gamelib/gameutil.h"
#include "gamelib/player.h"
#include "gamelib/platformmovement.h"

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

	World world = { 0 };
	world.level.scale = 10.0f;
	world.gravity = 100.0f;
	PlatformerLevel_LoadLayer(&world.level, 0, "res/maps/test.png");

	Player player = Player_Create();
	PhysicsComponent* playerPhys = Entity_PhysicsComponent(player.entity);
	playerPhys->collisionHull = (Rectangle){ -5.0f, -5.0f, 10.0f, 10.0f };
	playerPhys->collisionMask = 0xFFFFFFFF;
	const float playerSpeed = 500.0f;

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

		playerPhys->velocity.x = 0;

		if ( IsKeyDown(KEY_LEFT) )
		{
			playerPhys->velocity.x = -playerSpeed;
		}
		else if ( IsKeyDown(KEY_RIGHT) )
		{
			playerPhys->velocity.x = playerSpeed;
		}
		else
		{
			playerPhys->velocity.x = 0.0f;
		}

		PlatformMovement_MovePlayer(&player, &world);

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		Vector2i dims = PlatformerLevel_GetLayerDimensions(world.level, 0);

		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = PlatformerLevel_GetBlockWorldRectByCoOrds(world.level, (Vector2i){ x, y });
				Color blockColour = PlatformerLevel_GetBlockColourByCoOrds(world.level, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);
			}
		}

		Rectangle playerRect = PhysicsComponent_GetWorldCollisionHull(playerPhys);
		DrawRectangle((int)playerRect.x, (int)playerRect.y, (int)playerRect.width, (int)playerRect.height, RED);

		EndMode2D();

		char buffer[64];
		snprintf(buffer, sizeof(buffer), "Player: (%.2f, %.2f) [%.2f, %.2f]", playerPhys->position.x, playerPhys->position.y, playerPhys->velocity.x, playerPhys->velocity.y);
		DrawText(buffer, 10, 10, 10, BLACK);

		EndDrawing();
	}

	PlatformerLevel_Unload(world.level);
	CloseWindow();

	return 0;
}
