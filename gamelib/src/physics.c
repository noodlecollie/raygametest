#include "gamelib/physics.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "raymath.h"

static inline void MoveToPosition(Entity* entity, const TraceResult* result)
{
	PhysicsComponent* physComp = Entity_PhysicsComponent(entity);
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);

	entity->position.x += result->endPosition.x - hull.x;
	entity->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		entity->position = Vector2Add(entity->position, Vector2Scale(result->contactNormal, PHYSICS_CONTACT_ADJUST_DIST));
		physComp->velocity = (!result->endedColliding) ? Vector2ProjectAlongSurface(physComp->velocity, result->contactNormal) : Vector2Zero();
	}
}

void Physics_Simulate(World* world, Entity* entity)
{
	if ( !world || !entity )
	{
		return;
	}

	PhysicsComponent* physComp = Entity_PhysicsComponent(entity);

	if ( !physComp )
	{
		return;
	}

	float deltaTime = GetFrameTime();

	// Add in gravity applied over this time delta.
	physComp->velocity.y += world->gravity * physComp->gravityModifier * deltaTime;

	// Get how far we need to now move in this time delta.
	Vector2 delta = Vector2Scale(physComp->velocity, deltaTime);

	TraceResult result = TraceRectangleMovementInLevel(
		PhysicsComponent_GetWorldCollisionHull(physComp),
		delta,
		world->level,
		physComp->collisionMask
	);

	MoveToPosition(entity, &result);

	if ( result.collided && !result.endedColliding )
	{
		// For the remainder of the movement, slide along the surface.
		Vector2 remainderDelta = Vector2Scale(delta, fmaxf(1.0f - result.fraction, 0.0f));
		remainderDelta = Vector2ProjectAlongSurface(remainderDelta, result.contactNormal);

		TraceResult result2 = TraceRectangleMovementInLevel(
			PhysicsComponent_GetWorldCollisionHull(physComp),
			remainderDelta,
			world->level,
			physComp->collisionMask
		);

		MoveToPosition(entity, &result2);
	}
}
