#include <math.h>
#include "rendering/renderutils.h"

const float CAMERA_FAR_DEPTH = -500.0f;

float RenderUtils_GetDepthForLayer(DrawingLayer layer)
{
	static const float MIN_LAYER_DEPTH = 0.0f;
	static const float MAX_LAYER_DEPTH = (float)(((int)DLAYER__COUNT) - 1);

	const float layerAsFloat = (float)((int)layer);
	return fmaxf(MIN_LAYER_DEPTH, fminf(layerAsFloat, MAX_LAYER_DEPTH));
}
