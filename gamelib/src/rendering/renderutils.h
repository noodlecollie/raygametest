#pragma once

#include "gamelib/drawinglayers.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"

#define DEBUG_COLOUR_ENTITY RED
#define DEBUG_COLOUR_PHYSICS YELLOW

// Positive Z point into screen, negative Z points out.
extern const float CAMERA_FAR_DEPTH;
extern const float DEBUG_OVERLAY_DEPTH;

float RenderUtils_GetDepthForLayer(DrawingLayer layer);
float RenderUtils_GetDebugOverlayDepth(void);
Rectangle RenderUtils_CalcOpenGLTextureSubRect(Vector2i textureDim, Rectangle subRect);

// Scale is measured against camera's FOV
void DrawDebugScreenSpaceCross(Camera3D camera, Vector2 pos, float scale, Color colour);
