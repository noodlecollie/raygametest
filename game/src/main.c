#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "gamelib/platformerlevel.h"
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

	PlatformerLevel level = { 0 };
	level.scale = 10.0f;
	PlatformerLevel_LoadLayer(&level, 0, "res/maps/test.png");

	Player player = { 0 };
	player.collisionHull = (Rectangle){ -5.0f, -5.0f, 10.0f, 10.0f };
	const float playerSpeed = 5000.0f;
	const float gravity = 100.0f;

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		float deltaTime = GetFrameTime();

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

		player.velocity.x = 0;

		if ( IsKeyDown(KEY_LEFT) )
		{
			player.velocity.x -= playerSpeed * deltaTime;
		}

		if ( IsKeyDown(KEY_RIGHT) )
		{
			player.velocity.x += playerSpeed * deltaTime;
		}

		if ( !player.onGround )
		{
			player.velocity.y += gravity * deltaTime;
		}
		else
		{
			player.velocity.y = 0;
		}

		PlatformMovement_MovePlayer(&player, deltaTime, level, 0xFFFFFFFF);

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		Vector2i dims = PlatformerLevel_GetLayerDimensions(level, 0);

		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = PlatformerLevel_GetBlockWorldRectByCoOrds(level, (Vector2i){ x, y });
				Color blockColour = PlatformerLevel_GetBlockColourByCoOrds(level, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);
			}
		}

		Rectangle playerRect = Player_GetWorldCollisionHull(&player);
		DrawRectangle((int)playerRect.x, (int)playerRect.y, (int)playerRect.width, (int)playerRect.height, RED);

		EndMode2D();

		char buffer[64];
		snprintf(buffer, sizeof(buffer), "Player: (%.2f, %.2f) [%.2f, %.2f]", player.position.x, player.position.y, player.velocity.x, player.velocity.y);
		DrawText(buffer, 10, 10, 10, BLACK);

		EndDrawing();
	}

	PlatformerLevel_Unload(level);
	CloseWindow();

	return 0;
}
