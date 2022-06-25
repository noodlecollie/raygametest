#include "entity/terraincomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "rendering/terrainrenderer.h"

static inline bool LocValidForLayer(const TerrainComponentImpl* impl, size_t layer, Vector2i loc)
{
	TerrainDescriptor* descriptor = impl ? ResourcePool_GetTerrain(impl->terrainResource) : NULL;
	Vector2i dims = TerrainDescriptor_GetDimensionsInPixels(descriptor);

	return
		descriptor &&
		layer < TERRAIN_MAX_LAYERS &&
		loc.x >= 0 &&
		loc.x < dims.x &&
		loc.y >= 0 &&
		loc.y < dims.y;
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

	if ( impl->terrainResource )
	{
		ResourcePool_RemoveTerrainRef(impl->terrainResource);
		impl->terrainResource = NULL;
	}

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

Vector2i TerrainComponent_GetLayerDimensionsInPixels(const TerrainComponent* component, size_t layer)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS )
	{
		return (Vector2i){ 0, 0 };
	}

	return TerrainDescriptor_GetDimensionsInPixels(ResourcePool_GetTerrain(component->impl->terrainResource));
}

CollisionClass TerrainComponent_GetLayerCollisionClass(const TerrainComponent* component, size_t layer)
{
	if ( !component || layer >= TERRAIN_MAX_LAYERS )
	{
		return 0;
	}

	return TerrainDescriptor_GetLayerCollisionClass(ResourcePool_GetTerrain(component->impl->terrainResource), layer);
}

Color TerrainComponent_GetBlockColourByPixelLoc(const TerrainComponent* component, size_t layer, Vector2i loc)
{
	if ( !component || !LocValidForLayer(component->impl, layer, loc) )
	{
		return (Color){ 0, 0, 0, 0 };
	}

	return TerrainDescriptor_GetLayerColour(ResourcePool_GetTerrain(component->impl->terrainResource), layer, loc);
}

Rectangle TerrainComponent_GetBlockWorldRectByPixelLoc(const TerrainComponent* component, Vector2i loc)
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

	TerrainDescriptor* descriptor = ResourcePool_GetTerrain(component->impl->terrainResource);

	if ( !descriptor )
	{
		return rect;
	}

	Vector2i dims = TerrainDescriptor_GetDimensionsInPixels(descriptor);

	if ( loc.x < 0 || loc.x >= dims.x || loc.y < 0 || loc.y >= dims.y )
	{
		return rect;
	}

	rect.x = component->scale * (float)loc.x;
	rect.y = component->scale * (float)loc.y;
	rect.width = component->scale;
	rect.height = component->scale;

	return rect;
}

Vector2i TerrainComponent_PositionToPixelLoc(const TerrainComponent* component, Vector2 world)
{
	if ( !component )
	{
		return (Vector2i){ -1, -1 };
	}

	if ( component->scale == 0.0f )
	{
		return (Vector2i){ -1, -1 };
	}

	TerrainDescriptor* descriptor = ResourcePool_GetTerrain(component->impl->terrainResource);

	if ( !descriptor )
	{
		return (Vector2i){ -1, -1 };
	}

	world = Vector2Subtract(world, component->impl->ownerEntity->position);

	Vector2i loc = (Vector2i){ (int)(world.x / component->scale), (int)(world.y / component->scale) };
	Vector2i dims = TerrainDescriptor_GetDimensionsInPixels(descriptor);

	if ( loc.x >= dims.x || loc.y >= dims.y )
	{
		return (Vector2i){ -1, -1 };
	}

	return loc;
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
