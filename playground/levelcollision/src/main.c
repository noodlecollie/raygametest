#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"

#include "gamelib/platformerlevel.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"

typedef struct GuiValues
{
	float levelScale;
	bool snapToContactPositionClicked;
} GuiValues;

static inline bool PanKeyPressed()
{
	return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_SPACE);
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	int screenWidth = 800;
	int screenHeight = 450;

	const Rectangle collisionHull = { 0.0f, 0.0f, 10.0f, 20.0f };

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Level Collision");

	const Vector2 dpiScale = GetWindowScaleDPI();
	screenWidth = (int)((float)screenWidth * dpiScale.x);
	screenHeight = (int)((float)screenHeight * dpiScale.y);
	SetWindowSize(screenWidth, screenHeight);
	GuiSetStyle(DEFAULT, TEXT_SIZE, (int)((float)GuiGetStyle(DEFAULT, TEXT_SIZE) * dpiScale.x));

	GuiValues guiValues = { 0 };
	guiValues.levelScale = 30;

	const Rectangle guiBounds = { 0.0f, 0.0f, 240.0f * dpiScale.x, 120.0f * dpiScale.y };

	PlatformerLevel level = { 0 };
	level.scale = (float)guiValues.levelScale;
	PlatformerLevel_LoadLayer(&level, 0, "res/maps/test.png");
	Vector2i levelDim = PlatformerLevel_GetLayerDimensions(level, 0);

	Camera2D camera = { 0 };
	camera.target = (Vector2){ ((float)levelDim.x / 2.0f) * level.scale, ((float)levelDim.y / 2.0f) * level.scale };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	Vector2 beginPos = Vector2Zero();
	Vector2 endPos = Vector2Zero();

	SetTargetFPS(60);

	while ( !WindowShouldClose() )
	{
		level.scale = (float)guiValues.levelScale;

		bool mouseIsInGuiArea = CheckCollisionPointRec(GetMousePosition(), guiBounds);

		if ( !mouseIsInGuiArea )
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

			Vector2 panDrag = Vector2Zero();

			if ( IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && PanKeyPressed()) )
			{
				panDrag = GetMouseDelta();
			}

			camera.target = Vector2Subtract(camera.target, Vector2Scale(panDrag, 1.0f / camera.zoom));
		}

		if ( IsKeyPressed(KEY_R) )
		{
			camera.target = Vector2Zero();
			camera.zoom = 1.0f;
		}

		if ( !mouseIsInGuiArea )
		{
			if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !PanKeyPressed() )
			{
				beginPos = GetScreenToWorld2D(GetMousePosition(), camera);
			}

			if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) )
			{
				endPos = GetScreenToWorld2D(GetMousePosition(), camera);
			}
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

		if ( guiValues.snapToContactPositionClicked )
		{
			endHull = contactHull;
			endPos.x = RectangleMid(endHull).x;
			endPos.y = RectangleMid(endHull).y;

			traceResult.collided = false;

			guiValues.snapToContactPositionClicked = false;
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
				DrawRectangleLinesEx(beginHull, 1.0f / camera.zoom, GREEN);
				DrawCircle((int)beginPos.x, (int)beginPos.y, 3.0f, GREEN);

				DrawRectangleLinesEx(endHull, 1.0f / camera.zoom, BLUE);
				DrawCircle((int)endPos.x, (int)endPos.y, 3.0f, BLUE);

				DrawLineEx(beginPos, endPos, 1.0f / camera.zoom, traceResult.collided ? YELLOW : BLACK);

				if ( traceResult.collided )
				{
					DrawRectangleLinesEx(contactHull, 1.0f / camera.zoom, YELLOW);

					Vector2 contactMid = RectangleMid(contactHull);
					DrawCircle((int)contactMid.x, (int)contactMid.y, 3.0f, YELLOW);
				}
			}
		}

		EndMode2D();

		DrawRectangleLinesEx(guiBounds, dpiScale.x, BLACK);

		int fontSize = (int)(10.0f * dpiScale.y);
		int leftMargin = (int)(10.0f * dpiScale.x);

		int controlHeight = (int)(15.0f * dpiScale.y);
		int controlLeftMargin = (int)(70.0f * dpiScale.x);

		Rectangle guiRect = { (float)controlLeftMargin, 0.0f, 130.0f * dpiScale.x, (float)controlHeight };

		char buffer[64];

		snprintf(buffer, sizeof(buffer), "Left mouse: start point (%d, %d)", (int)beginPos.x, (int)beginPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(10.0f * dpiScale.y), fontSize, BLACK);

		snprintf(buffer, sizeof(buffer), "Right mouse: end point (%d, %d)", (int)endPos.x, (int)endPos.y);
		buffer[sizeof(buffer) - 1] = '\0';
		DrawText(buffer, leftMargin, (int)(30.0f * dpiScale.y), fontSize, BLACK);

		DrawText("Middle mouse: pan level", leftMargin, (int)(50.0f * dpiScale.y), fontSize, BLACK);

		guiRect.y = 70.0f * dpiScale.y;
		snprintf(buffer, sizeof(buffer), "%.2f", guiValues.levelScale);
		buffer[sizeof(buffer) - 1] = '\0';
		guiValues.levelScale = GuiSlider(guiRect, "Level scale: ", buffer, guiValues.levelScale, 1, 100);

		guiRect.y = 90.0f * dpiScale.y;
		guiValues.snapToContactPositionClicked = GuiButton(guiRect, "Snap to contact position");

		EndDrawing();
	}

	PlatformerLevel_Unload(level);
	CloseWindow();

	return 0;
}
