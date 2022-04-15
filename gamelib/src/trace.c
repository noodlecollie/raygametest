#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "raymath.h"

static void ClipTraceToLevelLayer(const Rectangle* hull, const Vector2* delta, const PlatformerLevel* level, size_t layer, TraceResult* result)
{
	Rectangle movementBounds = ExpandRectangle(*hull, *delta);

	if ( movementBounds.width <= 0.0f || movementBounds.height <= 0.0f )
	{
		// Nothing to test collisions with.
		return;
	}

	Vector2i blockMin = PlatformerLevel_PositionToCoOrds(*level, RectangleMin(movementBounds));
	Vector2i blockMax = PlatformerLevel_PositionToCoOrds(*level, RectangleMax(movementBounds));

	for ( int y = blockMin.y; y <= blockMax.y; ++y )
	{
		for ( int x = blockMin.x; x <= blockMax.x; ++x )
		{
			Vector2i blockCoOrds = { x, y };
			Color blockColour = PlatformerLevel_GetBlockColourByCoOrds(*level, layer, blockCoOrds);

			if ( blockColour.a == 0 )
			{
				// Transparent = block is empty.
				continue;
			}

			Rectangle blockHull = PlatformerLevel_GetBlockWorldRectByCoOrds(*level, blockCoOrds);
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
				result->contactNormal = contactNormal;
				result->contactPosition = (Vector2){ contact.x, contact.y };
				result->fraction = fraction;
				result->beganColliding = Vector2IsZero(contactNormal) || Vector2DotProduct(contactNormal, *delta) > 0.0f;
				result->endedColliding = result->beganColliding && result->fraction >= 1.0f;
			}
		}
	}
}

TraceResult TraceRectangleMovementInLevel(Rectangle hull, Vector2 delta, PlatformerLevel level, uint32_t collisionLayers)
{
	TraceResult result = { 0 };
	result.fraction = 1.0f;

	if ( collisionLayers != 0 )
	{
		for ( size_t layer = 0; layer < PLATFORMERLEVEL_MAX_LAYERS; ++layer )
		{
			if ( !(collisionLayers & (1 << layer)) )
			{
				continue;
			}

			ClipTraceToLevelLayer(&hull, &delta, &level, layer, &result);
		}
	}

	return result;
}