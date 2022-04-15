#include "raylib.h"
#include "raymath.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	const int screenWidth = 800;
	const int screenHeight = 450;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Level Collision");

	Camera2D camera = { 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		float deltaTime = GetFrameTime();

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		EndMode2D();

		DrawText("Testing", 10, 10, 10, BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
