#include <math.h>
#include <float.h>
#include "rendering/renderutils.h"
#include "gamelib/gameutil.h"

const float CAMERA_NEAR_DEPTH = -500.0f;
const float CAMERA_FAR_DEPTH = 500.0f;

Rectangle RenderUtils_CalcOpenGLTextureSubRect(Vector2i textureDim, Rectangle subRect)
{
	Rectangle textureRect = NormaliseRectangle((Rectangle){ 0.0f, 0.0f, (float)textureDim.x, (float)textureDim.y });
	Rectangle out = (Rectangle){ 0.0f, 0.0f, 0.0f, 0.0f };

	subRect = ClampRectangle(NormaliseRectangle(subRect), textureRect);

	if ( fabsf(textureRect.width) >= FLT_EPSILON )
	{
		out.x = subRect.x / textureRect.width;
		out.width = subRect.width / textureRect.width;
	}

	if ( fabsf(textureRect.height) >= FLT_EPSILON )
	{
		// OpenGL Y is in a different direction to bitmap Y.
		const float top = 1.0f - (subRect.y / textureRect.height);
		out.height = subRect.height / textureRect.height;
		out.y = top - out.height;
	}

	return out;
}

// TODO: Move to debugrendering
void DrawDebugScreenSpaceCross(Camera3D camera, Vector2 pos, float scale, Color colour)
{
	const float delta = scale * camera.fovy;

	DrawLine3D(
		(Vector3){ pos.x - delta, pos.y, (float)DRAWDEPTH_DEBUG },
		(Vector3){ pos.x + delta, pos.y, (float)DRAWDEPTH_DEBUG },
		colour
	);

	DrawLine3D(
		(Vector3){ pos.x, pos.y - delta, (float)DRAWDEPTH_DEBUG },
		(Vector3){ pos.x, pos.y + delta, (float)DRAWDEPTH_DEBUG },
		colour
	);
}
