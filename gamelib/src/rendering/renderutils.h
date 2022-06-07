#pragma once

#include "gamelib/drawinglayers.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"

extern const float CAMERA_FAR_DEPTH;

float RenderUtils_GetDepthForLayer(DrawingLayer layer);
Rectangle RenderUtils_CalcOpenGLTextureSubRect(Vector2i textureDim, Rectangle subRect);
