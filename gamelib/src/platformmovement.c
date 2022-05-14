#include "gamelib/platformmovement.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/physics.h"

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

static inline bool CheckIfStandingOnGround(Player* player, OldWorld* world)
{
	OldPhysicsComponent* component = OldEntity_PhysicsComponent(player->entity);
	Vector2 delta = (Vector2){ 0.0f, 2.0f * PHYSICS_CONTACT_ADJUST_DIST };
	Rectangle hull = OldPhysicsComponent_GetWorldCollisionHull(component);
	TraceResult result = TraceRectangleMovementInLevel(hull, delta, world->level, component->collisionMask);

	return result.collided && SurfaceNormalIsGround(result.contactNormal);
}

void PlatformMovement_MovePlayer(Player* player, OldWorld* world)
{
	if ( !player || !world || !OldEntity_PhysicsComponent(player->entity) )
	{
		return;
	}

	// Begin assuming the player will not be on the ground.
	player->onGround = false;

	Physics_SimulateOld(world, player->entity);

	if ( !player->onGround && CheckIfStandingOnGround(player, world) )
	{
		player->onGround = true;
	}
}
