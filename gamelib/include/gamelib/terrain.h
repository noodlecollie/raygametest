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

Terrain Terrain_Create(void);
void Terrain_Unload(Terrain terrain);

void Terrain_LoadLayer(Terrain* terrain, size_t layer, const char* fileName);
void Terrain_UnloadLayer(Terrain* terrain, size_t layer);
Vector2i Terrain_GetLayerDimensions(Terrain terrain, size_t layer);

Color Terrain_GetBlockColourByCoOrds(Terrain terrain, size_t layer, Vector2i coOrds);
Rectangle Terrain_GetBlockWorldRectByCoOrds(Terrain terrain, Vector2i coOrds);

Vector2i Terrain_PositionToCoOrds(Terrain terrain, Vector2 world);
