#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/entity/terraincomponent.h"
#include "descriptor/terraindescriptor.h"

static void ClipTraceToTerrainLayer(const Rectangle* hull, const Vector2* delta, const TerrainComponent* terrain, size_t layer, TraceResult* result)
{
	Rectangle movementBounds = ExpandRectangle(*hull, *delta);

	if ( movementBounds.width <= 0.0f || movementBounds.height <= 0.0f )
	{
		// Nothing to test collisions with.
		return;
	}

	Vector2i blockMin = TerrainComponent_PositionToPixelLoc(terrain, RectangleMin(movementBounds));
	Vector2i blockMax = TerrainComponent_PositionToPixelLoc(terrain, RectangleMax(movementBounds));

	for ( int y = blockMin.y; y <= blockMax.y; ++y )
	{
		for ( int x = blockMin.x; x <= blockMax.x; ++x )
		{
			Vector2i loc = { x, y };
			Color blockColour = TerrainComponent_GetBlockColourByPixelLoc(terrain, layer, loc);

			if ( blockColour.a == 0 )
			{
				// Transparent = block is empty.
				continue;
			}

			Rectangle blockHull = TerrainComponent_GetBlockWorldRectByPixelLoc(terrain, loc);
			Rectangle contact = { 0 };
			Vector2 contactNormal = { 0.0f, 0.0f };
			float fraction = 0.0f;

			if ( !RectSweep(*hull, blockHull, *delta, &contact, &contactNormal, &fraction) )
			{
				// No collision.
				continue;
			}

			if ( !result->collided || fraction < result->fraction )
			{
				result->collided = true;
				result->collisionEnt = TerrainComponent_GetOwnerEntity(terrain);
				result->contactNormal = contactNormal;
				result->endPosition = (Vector2){ contact.x, contact.y };
				result->fraction = fraction;
				result->beganColliding = Vector2IsZero(contactNormal) || Vector2DotProduct(contactNormal, *delta) > 0.0f;
				result->endedColliding = result->beganColliding && result->fraction >= 1.0f;
			}
		}
	}
}

TraceResult TraceResultNull(void)
{
	TraceResult result = { 0 };
	result.fraction = 1.0f;
	return result;
}

TraceResult TraceResultNoCollision(Vector2 hullEndPos)
{
	TraceResult result = { 0 };
	result.fraction = 1.0f;
	result.endPosition = hullEndPos;
	return result;
}

TraceResult TraceRectangleMovementAgainstTerrain(Rectangle hull, Vector2 delta, const struct TerrainComponent* terrain, Mask32 collisionLayers)
{
	TraceResult result = TraceResultNoCollision((Vector2){ hull.x + delta.x, hull.y + delta.y });

	if ( terrain && collisionLayers != 0 )
	{
		for ( size_t layer = 0; layer < TERRAIN_MAX_LAYERS; ++layer )
		{
			uint32_t collisionLayer = TerrainComponent_GetLayerCollisionLayer(terrain, layer);

			if ( !(collisionLayers & (1 << collisionLayer)) )
			{
				continue;
			}

			ClipTraceToTerrainLayer(&hull, &delta, terrain, layer, &result);
		}
	}

	return result;
}
