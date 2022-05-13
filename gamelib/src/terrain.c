#include <stdbool.h>
#include "gamelib/terrain.h"
#include "gamelib/gameutil.h"

#define MAX_IMAGE_WIDTH 1024
#define MAX_IMAGE_HEIGHT 256

typedef struct TerrainLayer
{
	Image image;
} TerrainLayer;

static inline bool CoOrdsValid(const Terrain* level, size_t layer, const Vector2i* coOrds)
{
	return
		level &&
		level->layers &&
		layer < TERRAIN_MAX_LAYERS &&
		coOrds->x >= 0 &&
		coOrds->x < level->layers[layer].image.width &&
		coOrds->y >= 0 &&
		coOrds->y < level->layers[layer].image.height;
}

static inline void InitLayersIfRequired(Terrain* level)
{
	if ( !level->layers )
	{
		level->layers = (TerrainLayer*)MemAlloc(TERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
	}
}

void Terrain_LoadLayer(Terrain* level, size_t layer, const char* fileName)
{
	if ( !level || layer >= TERRAIN_MAX_LAYERS || !fileName || !(*fileName) )
	{
		return;
	}

	InitLayersIfRequired(level);
	Terrain_UnloadLayer(level, layer);
	level->layers[layer].image = LoadImage(fileName);

	TraceLog(
		LOG_DEBUG,
		"TERRAIN: Loaded layer %zu (%dx%d) from %s",
		layer,
		level->layers[layer].image.width,
		level->layers[layer].image.height,
		fileName
	);
}

void Terrain_UnloadLayer(Terrain* level, size_t layer)
{
	if ( !level || layer >= TERRAIN_MAX_LAYERS || !level->layers || !level->layers[layer].image.data )
	{
		return;
	}

	UnloadImage(level->layers[layer].image);
	level->layers[layer].image = (Image){ 0 };
}

Vector2i Terrain_GetLayerDimensions(Terrain level, size_t layer)
{
	if ( layer >= TERRAIN_MAX_LAYERS || !level.layers )
	{
		return (Vector2i){ 0, 0 };
	}

	return (Vector2i){ level.layers[layer].image.width, level.layers[layer].image.height };
}

void Terrain_Unload(Terrain level)
{
	if ( !level.layers )
	{
		return;
	}

	for ( size_t index = 0; index < TERRAIN_MAX_LAYERS; ++index )
	{
		Terrain_UnloadLayer(&level, index);
	}

	MemFree(level.layers);
}

Color Terrain_GetBlockColourByCoOrds(Terrain level, size_t layer, Vector2i coOrds)
{
	if ( !CoOrdsValid(&level, layer, &coOrds) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(level.layers[layer].image, coOrds.x, coOrds.y);
}

Rectangle Terrain_GetBlockWorldRectByCoOrds(Terrain level, Vector2i coOrds)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( level.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return rect;
	}

	if ( coOrds.x < 0 || coOrds.y < 0 )
	{
		return rect;
	}

	rect.x = level.scale * (float)coOrds.x;
	rect.y = level.scale * (float)coOrds.y;
	rect.width = level.scale;
	rect.height = level.scale;

	return rect;
}

Vector2i Terrain_PositionToCoOrds(Terrain level, Vector2 world)
{
	if ( level.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return (Vector2i){ -1, -1 };
	}

	return (Vector2i){ (int)(world.x / level.scale), (int)(world.y / level.scale) };
}
