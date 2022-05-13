#pragma once

#include <stddef.h>
#include "raylib.h"
#include "gamelib/gametypes.h"

#define TERRAIN_MAX_LAYERS 32

struct TerrainLayer;

typedef struct Terrain
{
	struct TerrainLayer* layers;

	// World units per block
	float scale;
} Terrain;

void Terrain_Unload(Terrain level);

void Terrain_LoadLayer(Terrain* level, size_t layer, const char* fileName);
void Terrain_UnloadLayer(Terrain* level, size_t layer);
Vector2i Terrain_GetLayerDimensions(Terrain level, size_t layer);

Color Terrain_GetBlockColourByCoOrds(Terrain level, size_t layer, Vector2i coOrds);
Rectangle Terrain_GetBlockWorldRectByCoOrds(Terrain level, Vector2i coOrds);

Vector2i Terrain_PositionToCoOrds(Terrain level, Vector2 world);
