#include "gamelib/physics.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "raymath.h"

static inline void MoveToPosition(PhysicsComponent* component, const TraceResult* result)
{
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(component);

	component->position.x += result->endPosition.x - hull.x;
	component->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		component->position = Vector2Add(component->position, Vector2Scale(result->contactNormal, PHYSICS_CONTACT_ADJUST_DIST));
		component->velocity = (!result->endedColliding) ? Vector2ProjectAlongSurface(component->velocity, result->contactNormal) : Vector2Zero();
	}
}

void Physics_Simulate(World* world, PhysicsComponent* component)
{
	if ( !world || !component )
	{
		return;
	}

	float deltaTime = GetFrameTime();

	// Add in gravity applied over this time delta.
	component->velocity.y += world->gravity * component->gravityModifier * deltaTime;

	// Get how far we need to now move in this time delta.
	Vector2 delta = Vector2Scale(component->velocity, deltaTime);

	TraceResult result = TraceRectangleMovementInLevel(
		PhysicsComponent_GetWorldCollisionHull(component),
		delta,
		world->level,
		component->collisionMask
	);

	MoveToPosition(component, &result);

	if ( result.collided && !result.endedColliding )
	{
		// For the remainder of the movement, slide along the surface.
		Vector2 remainderDelta = Vector2Scale(delta, fmaxf(1.0f - result.fraction, 0.0f));
		remainderDelta = Vector2ProjectAlongSurface(remainderDelta, result.contactNormal);

		TraceResult result2 = TraceRectangleMovementInLevel(
			PhysicsComponent_GetWorldCollisionHull(component),
			remainderDelta,
			world->level,
			component->collisionMask
		);

		MoveToPosition(component, &result2);
	}
}
