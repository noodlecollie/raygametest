#include <stdio.h>
#include <math.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/parametric.h"

static inline bool PanKeyPressed()
{
	return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_SPACE);
}

static void DrawNumber(float t)
{
	char number[16];
	snprintf(number, sizeof(number), "%.2f", t);

	DrawText(number, 10, -30, 20, BLACK);
}

static void DrawScale(int row, float t, const char* label, float(* parametricFunc)(float))
{
	const float dotRadius = 8.0f;
	const int margin = 10;
	const int textMargin = 20;
	const int textSize = 20;
	const int width = 300;
	const int height = 24;
	const int left = margin;
	const int right = left + width;
	const int top = row * (height + margin);
	const int mid = top + (height / 2);
	const int bottom = top + height;

	DrawLine(left, mid, right, mid, BLACK);
	DrawLine(left, top, left, bottom, BLACK);
	DrawLine(right, top, right, bottom, BLACK);

	float x = parametricFunc(t);
	DrawCircle(left + (int)roundf(x * (float)width), mid, dotRadius, BLUE);

	DrawText(label, right + textMargin, mid - (textSize / 2), textSize, BLACK);
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	int screenWidth = 800;
	int screenHeight = 600;
	const double periodSecs = 2.0f;

	SetTraceLogLevel(LOG_DEBUG);
	InitWindow(screenWidth, screenHeight, "Parametric demo");

	const Vector2 dpiScale = GetWindowScaleDPI();
	screenWidth = (int)((float)screenWidth * dpiScale.x);
	screenHeight = (int)((float)screenHeight * dpiScale.y);
	SetWindowSize(screenWidth, screenHeight);

	Camera2D camera = { 0 };
	camera.rotation = 0.0f;
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.zoom = 1.0f;

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

		Vector2 panDrag = Vector2Zero();

		if ( IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && PanKeyPressed()) )
		{
			panDrag = GetMouseDelta();
		}

		camera.target = Vector2Subtract(camera.target, Vector2Scale(panDrag, 1.0f / camera.zoom));

		if ( IsKeyPressed(KEY_R) )
		{
			camera.target = Vector2Zero();
			camera.zoom = 1.0f;
		}

		BeginDrawing();

		ClearBackground(LIGHTGRAY);

		BeginMode2D(camera);

		float t = (float)(fmodl(GetTime(), periodSecs) / periodSecs);
		int row = 0;

		DrawNumber(t);
		DrawScale(row++, t, "Linear ramp up", &Parametric_LinearRampUp);
		DrawScale(row++, t, "Linear ramp down", &Parametric_LinearRampDown);
		DrawScale(row++, t, "Sine peak", &Parametric_SinePeak);
		DrawScale(row++, t, "Sine trough", &Parametric_SineTrough);
		DrawScale(row++, t, "Half sine up", &Parametric_HalfSineUp);
		DrawScale(row++, t, "Half sine down", &Parametric_HalfSineDown);
		DrawScale(row++, t, "Parabola ease in up", &Parametric_ParabolaEaseInUp);
		DrawScale(row++, t, "Parabola ease out up", &Parametric_ParabolaEaseOutUp);
		DrawScale(row++, t, "Parabola ease in down", &Parametric_ParabolaEaseInDown);
		DrawScale(row++, t, "Parabola ease out down", &Parametric_ParabolaEaseOutDown);
		DrawScale(row++, t, "Cubic ease in up", &Parametric_CubicEaseInUp);
		DrawScale(row++, t, "Cubic ease out up", &Parametric_CubicEaseOutUp);
		DrawScale(row++, t, "Cubic ease in down", &Parametric_CubicEaseInDown);
		DrawScale(row++, t, "Cubic ease out down", &Parametric_CubicEaseOutDown);

		EndMode2D();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
