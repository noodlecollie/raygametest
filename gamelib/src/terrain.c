#include <stdbool.h>
#include "gamelib/terrain.h"
#include "gamelib/gameutil.h"

#define MAX_IMAGE_WIDTH 1024
#define MAX_IMAGE_HEIGHT 256

typedef struct TerrainLayer
{
	Image image;
} TerrainLayer;

static inline bool CoOrdsValid(const Terrain* terrain, size_t layer, const Vector2i* coOrds)
{
	return
		terrain &&
		terrain->layers &&
		layer < OLDTERRAIN_MAX_LAYERS &&
		coOrds->x >= 0 &&
		coOrds->x < terrain->layers[layer].image.width &&
		coOrds->y >= 0 &&
		coOrds->y < terrain->layers[layer].image.height;
}

static inline void InitLayersIfRequired(Terrain* terrain)
{
	if ( !terrain->layers )
	{
		terrain->layers = (TerrainLayer*)MemAlloc(OLDTERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
	}
}

void Terrain_LoadLayer(Terrain* terrain, size_t layer, const char* fileName)
{
	if ( !terrain || layer >= OLDTERRAIN_MAX_LAYERS || !fileName || !(*fileName) )
	{
		return;
	}

	InitLayersIfRequired(terrain);
	Terrain_UnloadLayer(terrain, layer);
	terrain->layers[layer].image = LoadImage(fileName);

	TraceLog(
		LOG_DEBUG,
		"TERRAIN: Loaded layer %zu (%dx%d) from %s",
		layer,
		terrain->layers[layer].image.width,
		terrain->layers[layer].image.height,
		fileName
	);
}

void Terrain_UnloadLayer(Terrain* terrain, size_t layer)
{
	if ( !terrain || layer >= OLDTERRAIN_MAX_LAYERS || !terrain->layers || !terrain->layers[layer].image.data )
	{
		return;
	}

	UnloadImage(terrain->layers[layer].image);
	terrain->layers[layer].image = (Image){ 0 };
}

Vector2i Terrain_GetLayerDimensions(Terrain terrain, size_t layer)
{
	if ( layer >= OLDTERRAIN_MAX_LAYERS || !terrain.layers )
	{
		return (Vector2i){ 0, 0 };
	}

	return (Vector2i){ terrain.layers[layer].image.width, terrain.layers[layer].image.height };
}

Terrain Terrain_Create(void)
{
	Terrain terrain = { 0 };

	terrain.scale = 1.0f;

	return terrain;
}

void Terrain_Unload(Terrain terrain)
{
	if ( !terrain.layers )
	{
		return;
	}

	for ( size_t index = 0; index < OLDTERRAIN_MAX_LAYERS; ++index )
	{
		Terrain_UnloadLayer(&terrain, index);
	}

	MemFree(terrain.layers);
}

Color Terrain_GetBlockColourByCoOrds(Terrain terrain, size_t layer, Vector2i coOrds)
{
	if ( !CoOrdsValid(&terrain, layer, &coOrds) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(terrain.layers[layer].image, coOrds.x, coOrds.y);
}

Rectangle Terrain_GetBlockWorldRectByCoOrds(Terrain terrain, Vector2i coOrds)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( terrain.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return rect;
	}

	if ( coOrds.x < 0 || coOrds.y < 0 )
	{
		return rect;
	}

	rect.x = terrain.scale * (float)coOrds.x;
	rect.y = terrain.scale * (float)coOrds.y;
	rect.width = terrain.scale;
	rect.height = terrain.scale;

	return rect;
}

Vector2i Terrain_PositionToCoOrds(Terrain terrain, Vector2 world)
{
	if ( terrain.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return (Vector2i){ -1, -1 };
	}

	return (Vector2i){ (int)(world.x / terrain.scale), (int)(world.y / terrain.scale) };
}
