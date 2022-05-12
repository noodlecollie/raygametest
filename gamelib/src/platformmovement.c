#include "gamelib/platformmovement.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "raymath.h"

#define CONTACT_ADJUST_DIST 0.01f

static inline Vector2 ProjectAlongSurface(Vector2 delta, Vector2 surfaceNormal)
{
	const Vector2 parallelDir = Vector2PerpendicularClockwise(surfaceNormal);
	const float dotProduct = Vector2DotProduct(delta, parallelDir);
	return Vector2Scale(parallelDir, dotProduct);
}

static inline bool SurfaceNormalIsGround(Vector2 normal)
{
	return Vector2DotProduct(normal, (Vector2){ 0.0f, -1.0f }) > 0.5f;
}

static inline bool CheckIfStandingOnGround(Player* player, World* world, uint32_t collisionLayers)
{
	Vector2 delta = (Vector2){ 0.0f, 2.0f * CONTACT_ADJUST_DIST };
	Rectangle hull = Player_GetWorldCollisionHull(player);
	TraceResult result = TraceRectangleMovementInLevel(hull, delta, world->level, collisionLayers);

	return result.collided && SurfaceNormalIsGround(result.contactNormal);
}

static inline void MovePlayerToPosition(Player* player, const TraceResult* result)
{
	Rectangle hull = Player_GetWorldCollisionHull(player);

	hull.x = result->endPosition.x;
	hull.y = result->endPosition.y;

	player->position = RectangleMid(hull);

	if ( result->collided )
	{
		player->position = Vector2Add(player->position, Vector2Scale(result->contactNormal, CONTACT_ADJUST_DIST));
		player->velocity = (!result->endedColliding) ? ProjectAlongSurface(player->velocity, result->contactNormal) : Vector2Zero();
	}
}

void PlatformMovement_MovePlayer(Player* player, float deltaTime, World* world, uint32_t collisionLayers)
{
	if ( !player || !world )
	{
		return;
	}

	if ( !player->onGround )
	{
		player->velocity.y += world->gravity * deltaTime;
	}
	else if ( player->velocity.y > 0.0f )
	{
		player->velocity.y = 0.0f;
	}

	// Begin assuming the player will not be on the ground.
	player->onGround = false;

	Vector2 delta = Vector2Scale(player->velocity, deltaTime);
	TraceResult result = TraceRectangleMovementInLevel(Player_GetWorldCollisionHull(player), delta, world->level, collisionLayers);

	if ( result.collided )
	{
		MovePlayerToPosition(player, &result);

		if ( result.endedColliding )
		{
			player->onGround = true;
		}
		else
		{
			// For the remainder of the movement, slide along the surface.
			Vector2 remainderDelta = Vector2Scale(delta, fmaxf(1.0f - result.fraction, 0.0f));
			remainderDelta = ProjectAlongSurface(remainderDelta, result.contactNormal);

			TraceResult result2 = TraceRectangleMovementInLevel(Player_GetWorldCollisionHull(player), remainderDelta, world->level, collisionLayers);
			MovePlayerToPosition(player, &result2);
		}
	}
	else
	{
		player->position = Vector2Add(player->position, delta);
	}

	if ( !player->onGround && CheckIfStandingOnGround(player, world, collisionLayers) )
	{
		player->onGround = true;
	}
}
