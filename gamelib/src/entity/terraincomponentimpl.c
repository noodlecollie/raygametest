#include "entity/terraincomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "rendering/terrainrenderer.h"

static inline bool CoOrdsValid(const TerrainComponentImpl* impl, size_t layer, const Vector2i* coOrds)
{
	return
		impl &&
		impl->layers &&
		layer < TERRAIN_MAX_LAYERS &&
		coOrds->x >= 0 &&
		coOrds->x < impl->layers[layer].image.width &&
		coOrds->y >= 0 &&
		coOrds->y < impl->layers[layer].image.height;
}

static inline void InitLayersIfRequired(TerrainComponentImpl* impl)
{
	if ( !impl->layers )
	{
		impl->layers = (TerrainLayer*)MemAlloc(TERRAIN_MAX_LAYERS * sizeof(TerrainLayer));
	}
}

TerrainComponentImpl* TerrainComponentImpl_Create(struct Entity* ownerEntity)
{
	if ( !ownerEntity )
	{
		return NULL;
	}

	TerrainComponentImpl* impl = (TerrainComponentImpl*)MemAlloc(sizeof(TerrainComponentImpl));

	impl->component.impl = impl;
	impl->ownerEntity = ownerEntity;
	impl->component.scale = 1.0f;

	return impl;
}

void TerrainComponentImpl_Destroy(TerrainComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	TerrainComponent_Unload(&impl->component);
	MemFree(impl);
}

void TerrainComponentImpl_Render(TerrainComponentImpl* impl, Camera3D camera)
{
	TerrainRenderer_Draw(impl, camera);
}

struct Entity* TerrainComponent_GetOwnerEntity(const TerrainComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}

void TerrainComponent_Unload(TerrainComponent* component)
{
	if ( !component || !component->impl->layers )
	{
		return;
	}

	for ( size_t index = 0; index < TERRAIN_MAX_LAYERS; ++index )
	{
		TerrainComponent_UnloadLayer(component, index);
	}

	MemFree(component->impl->layers);
}

void TerrainComponent_LoadLayer(TerrainComponent* component, size_t layer, const char* fileName)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS || !fileName || !(*fileName) )
	{
		return;
	}

	InitLayersIfRequired(component->impl);
	TerrainComponent_UnloadLayer(component, layer);
	component->impl->layers[layer].image = LoadImage(fileName);

	TraceLog(
		LOG_INFO,
		"TERRAIN: Loaded layer %zu (%dx%d) from %s",
		layer,
		component->impl->layers[layer].image.width,
		component->impl->layers[layer].image.height,
		fileName
	);
}

void TerrainComponent_UnloadLayer(TerrainComponent* component, size_t layer)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS || !component->impl->layers || !component->impl->layers[layer].image.data )
	{
		return;
	}

	UnloadImage(component->impl->layers[layer].image);
	component->impl->layers[layer].image = (Image){ 0 };
}

Vector2i TerrainComponent_GetLayerDimensionsInPixels(const TerrainComponent* component, size_t layer)
{
	if ( layer >= TERRAIN_MAX_LAYERS || !component->impl->layers )
	{
		return (Vector2i){ 0, 0 };
	}

	return (Vector2i){ component->impl->layers[layer].image.width, component->impl->layers[layer].image.height };
}

Color TerrainComponent_GetBlockColourByCoOrds(const TerrainComponent* component, size_t layer, Vector2i coOrds)
{
	if ( !component || !CoOrdsValid(component->impl, layer, &coOrds) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return GetImageColor(component->impl->layers[layer].image, coOrds.x, coOrds.y);
}

Rectangle TerrainComponent_GetBlockWorldRectByCoOrds(const TerrainComponent* component, Vector2i coOrds)
{
	Rectangle rect = (Rectangle){ 0 };

	if ( !component )
	{
		return rect;
	}

	if ( component->scale == 0.0f )
	{
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
	if ( !component )
	{
		return (Vector2i){ -1, -1 };
	}

	if ( component->scale == 0.0f )
	{
		return (Vector2i){ -1, -1 };
	}

	world = Vector2Subtract(world, component->impl->ownerEntity->position);

	return (Vector2i){ (int)(world.x / component->scale), (int)(world.y / component->scale) };
}

bool TerrainComponent_SetTerrain(TerrainComponent* component, const char* filePath)
{
	if ( !component )
	{
		return false;
	}

	ResourcePoolTerrain* terrain = ResourcePool_LoadTerrainAndAddRef(filePath);

	if ( terrain != component->impl->terrainResource )
	{
		if ( component->impl->terrainResource )
		{
			ResourcePool_RemoveTerrainRef(component->impl->terrainResource);
		}

		component->impl->terrainResource = terrain;
	}
	else
	{
		// Terrain is the same, so remove the ref we just added.
		if ( terrain )
		{
			ResourcePool_RemoveTerrainRef(terrain);
		}
	}

	return component->impl->terrainResource != NULL;
}
