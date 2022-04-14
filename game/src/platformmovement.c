#include "platformmovement.h"
#include "gameutil.h"
#include "raymath.h"

static bool ClipMovementToBlock(
	Rectangle origHull,
	Vector2* delta,
	const PlatformerLevel* level,
	size_t layer,
	Vector2i blockCoOrds,
	Vector2* contactNormal)
{
	Color blockColour = PlatformerLevel_GetBlockColourByCoOrds(*level, layer, blockCoOrds);

	if ( blockColour.a == 0 )
	{
		// Transparent = block is empty.
		return false;
	}

	Rectangle blockHull = PlatformerLevel_GetBlockWorldRectByCoOrds(*level, blockCoOrds);

	if ( CheckCollisionRecs(origHull, blockHull) )
	{
		// Started out colliding - cannot move.
		*delta = Vector2Zero();
		return true;
	}

	Rectangle contact = { 0 };

	if ( !RectSweep(origHull, blockHull, *delta, &contact, contactNormal) )
	{
		// No collision.
		return false;
	}

	// Work out how much we should reduce the delta.
	float factor = Vector2Length((Vector2){ contact.x - origHull.x, contact.y - origHull.y }) / Vector2Length(*delta);
	*delta = Vector2Scale(*delta, factor);
}

static bool ClipMovement(
	Player* player,
	Vector2* delta,
	const PlatformerLevel* level,
	size_t layer,
	Vector2* contactNormal)
{
	Rectangle origHull = NormaliseRectangle(Player_GetWorldCollisionHull(player));
	Rectangle movementBounds = ExpandRectangle(origHull, *delta);

	if ( movementBounds.width <= 0.0f || movementBounds.height <= 0.0f )
	{
		// Nothing to test collisions with.
		return false;
	}

	Vector2i blockMin = PlatformerLevel_PositionToCoOrds(*level, RectangleMin(movementBounds));
	Vector2i blockMax = PlatformerLevel_PositionToCoOrds(*level, RectangleMax(movementBounds));
	bool collided = false;

	for ( int y = blockMin.y; y <= blockMax.y; ++y )
	{
		for ( int x = blockMin.x; x <= blockMax.x; ++x )
		{
			if ( ClipMovementToBlock(origHull, delta, level, layer, (Vector2i){ x, y }, contactNormal) )
			{
				collided = true;
			}

			if ( Vector2IsZero(*delta) )
			{
				// This block prevented the player from moving at all.
				// Just quit here.
				return collided;
			}
		}
	}

	return collided;
}

static bool ClipMovementToLayers(Player* player, const PlatformerLevel* level, uint32_t collisionLayers, Vector2* delta, Vector2* contactNormal)
{
	bool collided = false;

	for ( size_t layer = 0; layer < PLATFORMERLEVEL_MAX_LAYERS; ++layer )
	{
		if ( !(collisionLayers & (1 << layer)) )
		{
			continue;
		}

		if ( ClipMovement(player, delta, level, layer, contactNormal) )
		{
			collided = true;
		}
	}

	return collided;
}

void PlatformMovement_MovePlayer(Player* player, float deltaTime, PlatformerLevel level, uint32_t collisionLayers)
{
	if ( !player )
	{
		return;
	}

	Vector2 delta = Vector2Scale(player->velocity, deltaTime);

	if ( Vector2IsZero(delta) )
	{
		return;
	}

	if ( collisionLayers != 0 )
	{
		Vector2 clippedDelta = delta;
		Vector2 contactNormal = { 0.0f, 0.0f };

		bool collided = ClipMovementToLayers(player, &level, collisionLayers, &clippedDelta, &contactNormal);

		if ( Vector2IsZero(clippedDelta) )
		{
			// Cannot move.
			return;
		}

		if ( collided )
		{
			// We collided with something in the world. See if we can slide along the contact surface
			// to match our desired position in at least one of the axes.
			Vector2 contactPos = Vector2Add(player->position, clippedDelta);

			Vector2 slideDir = Vector2PerpendicularClockwise(contactNormal);

			if ( Vector2DotProduct(slideDir, delta) < 0.0f )
			{
				slideDir = Vector2Negate(slideDir);
			}

			Vector2 origTargetPos = Vector2Add(player->position, delta);
			Vector2 slideDelta = Vector2Subtract(origTargetPos, contactPos);
			slideDelta = Vector2Scale(slideDir, Vector2DotProduct(slideDelta, slideDir));

			// Run another collision check for the slide.
			ClipMovementToLayers(player, &level, collisionLayers, &slideDelta, &contactNormal);
			clippedDelta = Vector2Add(clippedDelta, slideDelta);
		}

		delta = clippedDelta;
	}

	player->position = Vector2Add(player->position, delta);
}
