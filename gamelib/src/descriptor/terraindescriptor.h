#pragma once

#include <stddef.h>
#include "gamelib/gametypes.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/drawinglayers.h"

#define TERRAIN_MAX_LAYERS 32

typedef struct TerrainDescriptor TerrainDescriptor;

TerrainDescriptor* TerrainDescriptor_LoadFromJSON(const char* filePath);
void TerrainDescriptor_Destroy(TerrainDescriptor* descriptor);

Vector2i TerrainDescriptor_GetDimensionsInPixels(TerrainDescriptor* descriptor);

Color TerrainDescriptor_GetLayerColour(TerrainDescriptor* descriptor, size_t layer, Vector2i pos);
Image* TerrainDescriptor_GetLayerImage(TerrainDescriptor* descriptor, size_t layer);
Texture2D* TerrainDescriptor_GetLayerTexture(TerrainDescriptor* descriptor, size_t layer);
DrawingLayer TerrainDescriptor_GetLayerDrawingLayer(TerrainDescriptor* descriptor, size_t layer);
uint32_t TerrainDescriptor_GetLayerCollisionLayer(TerrainDescriptor* descriptor, size_t layer);
