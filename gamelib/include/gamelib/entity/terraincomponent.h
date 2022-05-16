#pragma once

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

struct Entity* TerrainComponent_GetOwnerEntity(TerrainComponent* component);
void TerrainComponent_Unload(TerrainComponent* component);

void TerrainComponent_LoadLayer(TerrainComponent* component, size_t layer, const char* fileName);
void TerrainComponent_UnloadLayer(TerrainComponent* component, size_t layer);
Vector2i TerrainComponent_GetLayerDimensions(const TerrainComponent* component, size_t layer);

Color TerrainComponent_GetBlockColourByCoOrds(const TerrainComponent* component, size_t layer, Vector2i coOrds);
Rectangle TerrainComponent_GetBlockWorldRectByCoOrds(const TerrainComponent* component, Vector2i coOrds);

Vector2i TerrainComponent_PositionToCoOrds(const TerrainComponent* component, Vector2 world);
