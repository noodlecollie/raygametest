#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "gamelib/world.h"
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

	World world = { 0 };
	world.gravity = 2.0f;
	world.level.scale = 10.0f;
	PlatformerLevel_LoadLayer(&world.level, 0, "res/maps/test.png");
	Vector2i levelDim = PlatformerLevel_GetLayerDimensions(world.level, 0);

	Camera2D camera = { 0 };
	camera.target = (Vector2){ ((float)levelDim.x / 2.0f) * world.level.scale, ((float)levelDim.y / 2.0f) * world.level.scale };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	Player player = { 0 };
	player.collisionHull = (Rectangle){ -5.0f, -10.0f, 10.0f, 20.0f };
	player.position = (Vector2){ 1000.0f, 0.0f };

	Vector2 startPos = player.position;

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
			player.position = startPos;
			player.velocity = Vector2Zero();
			camera.zoom = 1.0f;
		}

		camera.target = player.position;

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
			movement.y -= 5.0f;
		}

		player.velocity = Vector2Scale(movement, 10.0f);
		PlatformMovement_MovePlayer(&player, 1.0f, &world, 0xFFFFFFFF);

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

			DrawRectangleRec(Player_GetWorldCollisionHull(&player), player.onGround ? YELLOW : RED);
		}

		EndMode2D();

		int leftMargin = (int)(10.0f * dpiScale.x);
		int fontSize = (int)(10.0f * dpiScale.y);

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Player position: (%.2f, %.2f)", player.position.x, player.position.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(10.0f * dpiScale.y), fontSize, BLUE);

		snprintf(buffer, sizeof(buffer), "Player velocity: (%.2f, %.2f)", player.velocity.x, player.velocity.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(30.0f * dpiScale.y), fontSize, BLUE);

		EndDrawing();
	}

	PlatformerLevel_Unload(world.level);
	CloseWindow();

	return 0;
}
