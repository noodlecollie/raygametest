#include <math.h>
#include <float.h>
#include "rendering/renderutils.h"
#include "gamelib/gameutil.h"

static const float FAR_LAYER_DEPTH = 0.0f;
static const float NEAR_LAYER_DEPTH = (float)((-(int)DLAYER__COUNT) - 1);

const float CAMERA_FAR_DEPTH = -500.0f;
const float DEBUG_OVERLAY_DEPTH = (float)(-(int)DLAYER__COUNT);

float RenderUtils_GetDepthForLayer(DrawingLayer layer)
{
	const float layerAsFloat = (float)(-(int)layer);
	return fmaxf(NEAR_LAYER_DEPTH, fminf(layerAsFloat, FAR_LAYER_DEPTH));
}

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

void DrawDebugScreenSpaceCross(Camera3D camera, Vector2 pos, float scale, Color colour)
{
	const float delta = scale * camera.fovy;

	DrawLine3D(
		(Vector3){ pos.x - delta, pos.y, DEBUG_OVERLAY_DEPTH },
		(Vector3){ pos.x + delta, pos.y, DEBUG_OVERLAY_DEPTH },
		colour
	);

	DrawLine3D(
		(Vector3){ pos.x, pos.y - delta, DEBUG_OVERLAY_DEPTH },
		(Vector3){ pos.x, pos.y + delta, DEBUG_OVERLAY_DEPTH },
		colour
	);
}
