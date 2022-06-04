#include "rendering/terrainrenderer.h"
#include "gamelib/drawinglayers.h"
#include "rendering/renderutils.h"

void TerrainRenderer_Draw(TerrainComponentImpl* impl, Camera3D camera)
{
	if ( !impl || !impl->layers )
	{
		return;
	}

	// TODO: Work out bounds of camera and only render blocks that intersect with these bounds.
	(void)camera;

	for ( size_t layer = 0; layer < TERRAIN_MAX_LAYERS; ++layer )
	{
		if ( !impl->layers[layer].image.data )
		{
			continue;
		}

		Vector2i dims = TerrainComponent_GetLayerDimensions(&impl->component, layer);

		// TODO: This should be improved as time goes on.
		// We could make use of instanced meshes here,
		// and we need things like textures etc. for blocks.
		// However, we will need to use 3D rendering rather
		// than 2D to make use of instancing, and to allow
		// proper depth checking.
		for ( int y = 0; y < dims.y; ++y )
		{
			for ( int x = 0; x < dims.x; ++x )
			{
				Rectangle blockRect = TerrainComponent_GetBlockWorldRectByCoOrds(&impl->component, (Vector2i){ x, y });
				Color blockColour = TerrainComponent_GetBlockColourByCoOrds(&impl->component, 0, (Vector2i){ x, y });
				DrawRectangleRec(blockRect, blockColour);

				// TODO: Allow layers to be set manually
				const float depth = RenderUtils_GetDepthForLayer(DLAYER_BACKGROUND);

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
