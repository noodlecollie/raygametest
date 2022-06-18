#pragma once

#include "gamelib/entity/terraincomponent.h"
#include "descriptor/terraindescriptor.h"
#include "resourcepool/resourcepool.h"

struct Entity;

typedef struct TerrainComponentImpl
{
	struct Entity* ownerEntity;
	TerrainComponent component;
	ResourcePoolTerrain* terrainResource;
} TerrainComponentImpl;

TerrainComponentImpl* TerrainComponentImpl_Create(struct Entity* ownerEntity);
void TerrainComponentImpl_Destroy(TerrainComponentImpl* impl);
void TerrainComponentImpl_Render(TerrainComponentImpl* impl, Camera3D camera);
