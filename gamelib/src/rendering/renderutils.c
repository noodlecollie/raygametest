#include <math.h>
#include <float.h>
#include "rendering/renderutils.h"
#include "gamelib/gameutil.h"

const float CAMERA_FAR_DEPTH = -500.0f;

float RenderUtils_GetDepthForLayer(DrawingLayer layer)
{
	static const float MIN_LAYER_DEPTH = 0.0f;
	static const float MAX_LAYER_DEPTH = (float)(((int)DLAYER__COUNT) - 1);

	const float layerAsFloat = (float)((int)layer);
	return fmaxf(MIN_LAYER_DEPTH, fminf(layerAsFloat, MAX_LAYER_DEPTH));
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
		out.y = 1.0f - (subRect.y / textureRect.height);
		out.height = subRect.height / textureRect.height;
	}

	return out;
}
