#pragma once

#include "gamelib/entity/terraincomponent.h"

struct Entity;

typedef struct TerrainLayer
{
	Image image;
} TerrainLayer;

typedef struct TerrainComponentImpl
{
	struct Entity* ownerEntity;
	TerrainComponent component;
	TerrainLayer* layers;
} TerrainComponentImpl;

TerrainComponentImpl* TerrainComponentImpl_Create(struct Entity* ownerEntity);
void TerrainComponentImpl_Destroy(TerrainComponentImpl* impl);
