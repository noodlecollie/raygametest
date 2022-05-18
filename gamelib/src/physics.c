#include <float.h>
#include "gamelib/physics.h"
#include "gamelib/oldentity/oldphysicscomponent.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/entity/entity.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/logiccomponent.h"
#include "gamelib/world.h"

static inline Vector2 ContactPosition(Vector2 pos, Vector2 normal)
{
	return Vector2Add(pos, Vector2Scale(normal, PHYSICS_CONTACT_ADJUST_DIST));
}

static inline void MoveToPositionOld(OldEntity* entity, const TraceResult* result)
{
	OldPhysicsComponent* physComp = OldEntity_PhysicsComponent(entity);
	Rectangle hull = OldPhysicsComponent_GetWorldCollisionHull(physComp);

	entity->position.x += result->endPosition.x - hull.x;
	entity->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		entity->position = ContactPosition(entity->position, result->contactNormal);
		physComp->velocity = (!result->endedColliding) ? Vector2ProjectAlongSurface(physComp->velocity, result->contactNormal) : Vector2Zero();
	}
}

void Physics_SimulateOld(OldWorld* world, OldEntity* entity)
{
	if ( !world || !entity )
	{
		return;
	}

	OldPhysicsComponent* physComp = OldEntity_PhysicsComponent(entity);

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
		OldPhysicsComponent_GetWorldCollisionHull(physComp),
		delta,
		world->level,
		physComp->collisionMask
	);

	MoveToPositionOld(entity, &result);

	if ( result.collided && !result.endedColliding )
	{
		// For the remainder of the movement, slide along the surface.
		Vector2 remainderDelta = Vector2Scale(delta, fmaxf(1.0f - result.fraction, 0.0f));
		remainderDelta = Vector2ProjectAlongSurface(remainderDelta, result.contactNormal);

		TraceResult result2 = TraceRectangleMovementInLevel(
			OldPhysicsComponent_GetWorldCollisionHull(physComp),
			remainderDelta,
			world->level,
			physComp->collisionMask
		);

		MoveToPositionOld(entity, &result2);
	}
}

static inline size_t MaxMovementIterations(PhysicsMovementType movementType)
{
	switch ( movementType )
	{
		case PHYSMOVE_SLIDE:
		{
			return 2;
		}

		default:
		{
			return 0;
		}
	}
}

static void ApplyPerFrameDeltas(const World* world, PhysicsComponent* physComp)
{
	float deltaTime = GetFrameTime();

	// Add in gravity applied over this time delta.
	physComp->velocity.y += world->gravity * physComp->gravityModifier * deltaTime;
}

static void MoveToPosition(PhysicsComponent* physComp, const TraceResult* result)
{
	Entity* ent = PhysicsComponent_GetOwnerEntity(physComp);
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);

	// The result end position is the top left of the hull, rather than the player's
	// actual position, which will probably be different (and may not even be the centre
	// of the collision hull if the hull has a weird offset). Just apply the delta.
	ent->position.x += result->endPosition.x - hull.x;
	ent->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		ent->position = ContactPosition(ent->position, result->contactNormal);
		physComp->velocity = (!result->endedColliding) ? Vector2ProjectAlongSurface(physComp->velocity, result->contactNormal) : Vector2Zero();
	}
}

static void FireCollisionCallbacks(PhysicsComponent* physComp, const TraceResult* result, size_t iteration)
{
	if ( !result->collided || !result->collisionEnt )
	{
		return;
	}

	OnPhysicsCollidedArgs callbackArgs = { 0 };

	callbackArgs.initiator = PhysicsComponent_GetOwnerEntity(physComp);
	callbackArgs.initiatorComponentType = COMPONENT_PHYSICS;
	callbackArgs.recipient = result->collisionEnt;
	callbackArgs.recipientComponentType = result->collisionComponentType;
	callbackArgs.simIteration = iteration;

	for ( LogicComponent* logic = Entity_GetLogicComponentListHead(callbackArgs.initiator); logic; logic = Entity_GetNextLogicComponent(logic) )
	{
		if ( logic->callbacks.onPhysicsCollided )
		{
			logic->callbacks.onPhysicsCollided(logic, &callbackArgs);
		}
	}

	for ( LogicComponent* logic = Entity_GetLogicComponentListHead(callbackArgs.recipient); logic; logic = Entity_GetNextLogicComponent(logic) )
	{
		if ( logic->callbacks.onPhysicsCollided )
		{
			logic->callbacks.onPhysicsCollided(logic, &callbackArgs);
		}
	}
}

void Physics_SimulateObjectInWorld(struct World* world, struct PhysicsComponent* physComp)
{
	if ( !physComp ||
	     !world ||
	     !physComp->enabled ||
	     physComp->movementType == PHYSMOVE_NONE ||
	     GetFrameTime() == 0.0f )
	{
		return;
	}

	ApplyPerFrameDeltas(world, physComp);

	const size_t maxIterations = MaxMovementIterations(physComp->movementType);

	for ( size_t iteration = 0; iteration < maxIterations; ++iteration )
	{
		TraceResult result = Physics_TraceHullInWorld(
			world,
			PhysicsComponent_GetWorldCollisionHull(physComp),
			Vector2Scale(physComp->velocity, GetFrameTime()),
			physComp->collisionMask,
			PhysicsComponent_GetOwnerEntity(physComp)
		);

		MoveToPosition(physComp, &result);
		FireCollisionCallbacks(physComp, &result, iteration);

		if ( !result.collided )
		{
			break;
		}
	}
}

TraceResult Physics_TraceHullInWorld(
	struct World* world,
	Rectangle hull,
	Vector2 delta,
	Mask32 collisionMask,
	struct Entity* hullOwner
)
{
	if ( !world )
	{
		return TraceResultNull();
	}

	// Get how far we need to move in this time delta.
	Vector2 hullPos = (Vector2){ hull.x, hull.y };

	// Find the final contact position from the collision with the nearest component.
	float shortestFraction = FLT_MAX;
	TraceResult shortestTraceResult = TraceResultNoCollision(Vector2Add(hullPos, delta));

	// TODO: This is a naive search. It should be optimised with use of a quadtree.
	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		if ( ent == hullOwner )
		{
			continue;
		}

		if ( Entity_GetTerrainComponent(ent) )
		{
			TraceResult workingResult = TraceRectangleMovementAgainstTerrain(
				hull,
				delta,
				Entity_GetTerrainComponent(ent),
				collisionMask
			);

			if ( workingResult.fraction < shortestFraction )
			{
				shortestFraction = workingResult.fraction;
				shortestTraceResult = workingResult;
			}
		}
	}

	return shortestTraceResult;
}
