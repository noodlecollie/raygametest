#include <stdbool.h>
#include "platformerlevel.h"
#include "gameutil.h"

#define MAX_IMAGE_WIDTH 1024
#define MAX_IMAGE_HEIGHT 256

typedef struct PlatformerLevelLayer
{
	Image image;
} PlatformerLevelLayer;

static inline bool CoOrdsValid(const PlatformerLevel* level, size_t layer, const Vector2i* coOrds)
{
	return
		level &&
		level->layers &&
		layer < PLATFORMERLEVEL_MAX_LAYERS &&
		coOrds->x >= 0 &&
		coOrds->x < level->layers[layer].image.width &&
		coOrds->y >= 0 &&
		coOrds->y < level->layers[layer].image.height;
}

static inline void InitLayersIfRequired(PlatformerLevel* level)
{
	if ( !level->layers )
	{
		level->layers = (PlatformerLevelLayer*)MemAlloc(PLATFORMERLEVEL_MAX_LAYERS * sizeof(PlatformerLevelLayer));
	}
}

void PlatformerLevel_LoadLayer(PlatformerLevel* level, size_t layer, const char* fileName)
{
	if ( !level || layer >= PLATFORMERLEVEL_MAX_LAYERS || !fileName || !(*fileName) )
	{
		return;
	}

	InitLayersIfRequired(level);
	PlatformerLevel_UnloadLayer(level, layer);
	level->layers[layer].image = LoadImage(fileName);

	TraceLog(
		LOG_DEBUG,
		"PLATFORMERLEVEL: Loaded layer %zu (%dx%d) from %s",
		layer,
		level->layers[layer].image.width,
		level->layers[layer].image.height,
		fileName
	);
}

void PlatformerLevel_UnloadLayer(PlatformerLevel* level, size_t layer)
{
	if ( !level || layer >= PLATFORMERLEVEL_MAX_LAYERS || !level->layers || !level->layers[layer].image.data )
	{
		return;
	}

	UnloadImage(level->layers[layer].image);
	level->layers[layer].image = (Image){ 0 };
}

Vector2i PlatformerLevel_GetLayerDimensions(PlatformerLevel level, size_t layer)
{
	if ( layer >= PLATFORMERLEVEL_MAX_LAYERS || !level.layers )
	{
		return (Vector2i){ 0, 0 };
	}

	return (Vector2i){ level.layers[layer].image.width, level.layers[layer].image.height };
}

void PlatformerLevel_Unload(PlatformerLevel level)
{
	if ( !level.layers )
	{
		return;
	}

	for ( size_t index = 0; index < PLATFORMERLEVEL_MAX_LAYERS; ++index )
	{
		PlatformerLevel_UnloadLayer(&level, index);
	}

	MemFree(level.layers);
}

Color PlatformerLevel_GetBlockColourByCoOrds(PlatformerLevel level, size_t layer, Vector2i coOrds)
{
	if ( !CoOrdsValid(&level, layer, &coOrds) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(level.layers[layer].image, coOrds.x, coOrds.y);
}

Rectangle PlatformerLevel_GetBlockWorldRectByCoOrds(PlatformerLevel level, Vector2i coOrds)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( level.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "PLATFORMERLEVEL: Scale was zero!");
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

Vector2i PlatformerLevel_PositionToCoOrds(PlatformerLevel level, Vector2 world)
{
	if ( level.scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "PLATFORMERLEVEL: Scale was zero!");
		return (Vector2i){ -1, -1 };
	}

	return (Vector2i){ (int)(world.x / level.scale), (int)(world.y / level.scale) };
}
