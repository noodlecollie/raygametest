#pragma once

#include "gamelib/drawingdepth.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/gametypes.h"

#define DEBUG_COLOUR_ENTITY RED
#define DEBUG_COLOUR_PHYSICS YELLOW

// Positive Z point into screen, negative Z points out.
// If camera position is set to CAMERA_NEAR_DEPTH, then
// the area in which objects may be drawn will be between
// CAMERA_NEAR DEPTH and CAMERA_FAR_DEPTH.
extern const float CAMERA_NEAR_DEPTH;
extern const float CAMERA_FAR_DEPTH;

float RenderUtils_GetWorldDepthFromDrawingDepth(DrawingDepth drawingDepth);
Rectangle RenderUtils_CalcOpenGLTextureSubRect(Vector2i textureDim, Rectangle subRect);

// Scale is measured against camera's FOV
void DrawDebugScreenSpaceCross(Camera3D camera, Vector2 pos, float scale, Color colour);
