#include "raylib.h"
#include "raymath.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

	Camera2D camera = { 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

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
		DrawRectangle(-5, -5, 10, 10, RED);
		EndMode2D();

		DrawText("Some text", 20, 20, 10, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
