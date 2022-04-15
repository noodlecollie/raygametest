#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "gamelib/platformerlevel.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	const int screenWidth = 800;
	const int screenHeight = 450;

	const Rectangle collisionHull = { 0.0f, 0.0f, 10.0f, 20.0f };

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Level Collision");

	Camera2D camera = { 0 };
	camera.target = (Vector2){ 0, 0 };
	camera.offset = (Vector2){ 0, 0 };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	PlatformerLevel level = { 0 };
	level.scale = 30.0f;
	PlatformerLevel_LoadLayer(&level, 0, "res/maps/test.png");

	Vector2 beginPos = Vector2Zero();
	Vector2 endPos = Vector2Zero();

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		Vector2 panDrag = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ? GetMouseDelta() : Vector2Zero();
		camera.offset = Vector2Add(camera.offset, panDrag);

		if ( IsKeyPressed(KEY_R) )
		{
			camera.offset = Vector2Zero();
		}

		if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) )
		{
			beginPos = GetScreenToWorld2D(GetMousePosition(), camera);
		}

		if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) )
		{
			endPos = GetScreenToWorld2D(GetMousePosition(), camera);
		}

		Vector2 traceDelta = Vector2Subtract(endPos, beginPos);
		Rectangle beginHull = { beginPos.x - (collisionHull.width / 2.0f), beginPos.y - (collisionHull.height / 2.0f), collisionHull.width, collisionHull.height };

		Rectangle endHull = beginHull;
		endHull.x += traceDelta.x;
		endHull.y += traceDelta.y;

		Rectangle contactHull = endHull;

		TraceResult traceResult = { 0 };

		if ( Vector2Length(traceDelta) > 0.0f )
		{
			traceResult = TraceRectangleMovementInLevel(beginHull, traceDelta, level, 1 << 0);

			if ( traceResult.collided )
			{
				contactHull.x = traceResult.contactPosition.x;
				contactHull.y = traceResult.contactPosition.y;
			}
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

			if ( Vector2Length(traceDelta) > 0.0f )
			{
				DrawRectangleLinesEx(beginHull, 1.0f, GREEN);
				DrawCircle((int)beginPos.x, (int)beginPos.y, 3.0f, GREEN);

				DrawRectangleLinesEx(endHull, 1.0f, BLUE);
				DrawCircle((int)endPos.x, (int)endPos.y, 3.0f, BLUE);

				DrawLineEx(beginPos, endPos, 1.0f, traceResult.collided ? YELLOW : BLACK);

				if ( traceResult.collided )
				{
					DrawRectangleLinesEx(contactHull, 1.0f, YELLOW);

					Vector2 contactMid = RectangleMid(contactHull);
					DrawCircle((int)contactMid.x, (int)contactMid.y, 3.0f, YELLOW);
				}
			}
		}

		EndMode2D();

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Left mouse: start point (%d, %d)", (int)beginPos.x, (int)beginPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, 10, 10, 10, BLACK);

		snprintf(buffer, sizeof(buffer), "Right mouse: end point (%d, %d)", (int)endPos.x, (int)endPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, 10, 30, 10, BLACK);

		DrawText("Middle mouse: pan level", 10, 50, 10, BLACK);

		EndDrawing();
	}

	PlatformerLevel_Unload(level);
	CloseWindow();

	return 0;
}
