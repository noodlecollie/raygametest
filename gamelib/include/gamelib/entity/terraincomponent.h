#pragma once

#include <stdbool.h>
#include "gamelib/gametypes.h"
#include "gamelib/external/raylibheaders.h"

#define TERRAIN_MAX_LAYERS MASK32_BITS

struct Entity;
struct TerrainComponentImpl;

typedef struct TerrainComponent
{
	struct TerrainComponentImpl* impl;

	// World units per block
	float scale;
} TerrainComponent;

struct Entity* TerrainComponent_GetOwnerEntity(const TerrainComponent* component);
void TerrainComponent_Unload(TerrainComponent* component);

void TerrainComponent_LoadLayer(TerrainComponent* component, size_t layer, const char* fileName);
void TerrainComponent_UnloadLayer(TerrainComponent* component, size_t layer);
Vector2i TerrainComponent_GetLayerDimensionsInPixels(const TerrainComponent* component, size_t layer);

Color TerrainComponent_GetBlockColourByPixelLoc(const TerrainComponent* component, size_t layer, Vector2i loc);
Rectangle TerrainComponent_GetBlockWorldRectByPixelLoc(const TerrainComponent* component, Vector2i loc);

Vector2i TerrainComponent_PositionToPixelLoc(const TerrainComponent* component, Vector2 world);

bool TerrainComponent_SetTerrain(TerrainComponent* component, const char* filePath);
