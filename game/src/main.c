#include "raylib.h"
#include "raymath.h"
#include "platformerlevel.h"

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

		EndMode2D();

		DrawText("Some text", 20, 20, 10, BLACK);

		EndDrawing();
	}

	PlatformerLevel_Unload(level);
	CloseWindow();

	return 0;
}
