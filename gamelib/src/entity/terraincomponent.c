#include <stdbool.h>
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/gameutil.h"
#include "gamelib/entity/entity.h"

#define MAX_IMAGE_WIDTH 1024
#define MAX_IMAGE_HEIGHT 256

typedef struct TerrainLayer
{
	Image image;
} TerrainLayer;

static inline bool CoOrdsValid(const TerrainComponent* component, size_t layer, const Vector2i* coOrds)
{
	return
		component &&
		component->layers &&
		layer < TERRAIN_MAX_LAYERS &&
		coOrds->x >= 0 &&
		coOrds->x < component->layers[layer].image.width &&
		coOrds->y >= 0 &&
		coOrds->y < component->layers[layer].image.height;
}

static inline void InitLayersIfRequired(TerrainComponent* component)
{
	if ( !component->layers )
	{
		component->layers = (TerrainLayer*)MemAlloc(TERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
	}
}

void TerrainComponent_Unload(TerrainComponent* component)
{
	if ( !component->layers )
	{
		return;
	}

	for ( size_t index = 0; index < TERRAIN_MAX_LAYERS; ++index )
	{
		TerrainComponent_UnloadLayer(component, index);
	}

	MemFree(component->layers);
}

void TerrainComponent_LoadLayer(TerrainComponent* component, size_t layer, const char* fileName)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS || !fileName || !(*fileName) )
	{
		return;
	}

	InitLayersIfRequired(component);
	TerrainComponent_UnloadLayer(component, layer);
	component->layers[layer].image = LoadImage(fileName);

	TraceLog(
		LOG_DEBUG,
		"TERRAIN: Loaded layer %zu (%dx%d) from %s",
		layer,
		component->layers[layer].image.width,
		component->layers[layer].image.height,
		fileName
	);
}

void TerrainComponent_UnloadLayer(TerrainComponent* component, size_t layer)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS || !component->layers || !component->layers[layer].image.data )
	{
		return;
	}

	UnloadImage(component->layers[layer].image);
	component->layers[layer].image = (Image){ 0 };
}

Vector2i TerrainComponent_GetLayerDimensions(const TerrainComponent* component, size_t layer)
{
	if ( layer >= TERRAIN_MAX_LAYERS || !component->layers )
	{
		return (Vector2i){ 0, 0 };
	}

	return (Vector2i){ component->layers[layer].image.width, component->layers[layer].image.height };
}

Color TerrainComponent_GetBlockColourByCoOrds(const TerrainComponent* component, size_t layer, Vector2i coOrds)
{
	if ( !CoOrdsValid(component, layer, &coOrds) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(component->layers[layer].image, coOrds.x, coOrds.y);
}

Rectangle TerrainComponent_GetBlockWorldRectByCoOrds(const TerrainComponent* component, Vector2i coOrds)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( component->scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return rect;
	}

	if ( coOrds.x < 0 || coOrds.y < 0 )
	{
		return rect;
	}

	rect.x = component->scale * (float)coOrds.x;
	rect.y = component->scale * (float)coOrds.y;
	rect.width = component->scale;
	rect.height = component->scale;

	return rect;
}

Vector2i TerrainComponent_PositionToCoOrds(const TerrainComponent* component, Vector2 world)
{
	if ( component->scale == 0.0f )
	{
		TraceLog(LOG_DEBUG, "TERRAIN: Scale was zero!");
		return (Vector2i){ -1, -1 };
	}

	world = Vector2Subtract(world, component->ownerEntity->position);

	return (Vector2i){ (int)(world.x / component->scale), (int)(world.y / component->scale) };
}
