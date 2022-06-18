#include "rendering/terrainrenderer.h"
#include "gamelib/drawinglayers.h"
#include "rendering/renderutils.h"

void TerrainRenderer_Draw(TerrainComponentImpl* impl, Camera3D camera)
{
	if ( !impl || !impl->terrainResource )
	{
		return;
	}

	TerrainDescriptor* descriptor = ResourcePool_GetTerrain(impl->terrainResource);

	if ( !descriptor )
	{
		return;
	}

	Vector2i dims = TerrainDescriptor_GetDimensionsInPixels(descriptor);

	// TODO: Work out bounds of camera and only render blocks that intersect with these bounds.
	(void)camera;

	for ( size_t layer = 0; layer < TERRAIN_MAX_LAYERS; ++layer )
	{
		// TODO: This should be improved as time goes on.
		// We could make use of instanced meshes here,
		// and we need things like textures etc. for blocks.
		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Color blockColour = TerrainComponent_GetBlockColourByPixelLoc(&impl->component, 0, (Vector2i){ x, y });

				if ( blockColour.a == 0 )
				{
					continue;
				}

				Rectangle blockRect = TerrainComponent_GetBlockWorldRectByPixelLoc(&impl->component, (Vector2i){ x, y });
				DrawingLayer dLayer = TerrainDescriptor_GetLayerDrawingLayer(descriptor, layer);
				const float depth = RenderUtils_GetDepthForLayer(dLayer);

				DrawTriangle3D(
					(Vector3){ blockRect.x, blockRect.y, depth },
					(Vector3){ blockRect.x, blockRect.y + blockRect.height, depth },
					(Vector3){ blockRect.x + blockRect.width, blockRect.y + blockRect.height, depth },
					blockColour
				);

				DrawTriangle3D(
					(Vector3){ blockRect.x, blockRect.y, depth },
					(Vector3){ blockRect.x + blockRect.width, blockRect.y + blockRect.height, depth },
					(Vector3){ blockRect.x + blockRect.width, blockRect.y, depth },
					blockColour
				);
			}
		}
	}
}
