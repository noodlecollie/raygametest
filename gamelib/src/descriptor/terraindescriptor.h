#pragma once

#include <stddef.h>
#include "gamelib/gametypes.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/drawingdepth.h"
#include "gamelib/collisionclasses.h"

#define TERRAIN_MAX_LAYERS 32

typedef struct TerrainDescriptor TerrainDescriptor;

TerrainDescriptor* TerrainDescriptor_LoadFromJSONFile(const char* filePath);
void TerrainDescriptor_Destroy(TerrainDescriptor* descriptor);

Vector2i TerrainDescriptor_GetDimensionsInPixels(TerrainDescriptor* descriptor);

Color TerrainDescriptor_GetLayerColour(TerrainDescriptor* descriptor, size_t layer, Vector2i pos);
Image* TerrainDescriptor_GetLayerImage(TerrainDescriptor* descriptor, size_t layer);
Texture2D* TerrainDescriptor_GetLayerTexture(TerrainDescriptor* descriptor, size_t layer);
DrawingDepth TerrainDescriptor_GetLayerDrawingDepth(TerrainDescriptor* descriptor, size_t layer);
CollisionClass TerrainDescriptor_GetLayerCollisionClass(TerrainDescriptor* descriptor, size_t layer);
