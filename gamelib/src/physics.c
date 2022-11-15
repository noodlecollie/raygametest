#include <float.h>
#include "gamelib/physics.h"
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

static inline size_t MaxMovementIterations(PhysicsMovementType movementType)
{
	switch ( movementType )
	{
		case PHYSMOVE_NONE:
		{
			return 0;
		}

		case PHYSMOVE_STICK:
		{
			return 1;
		}

		case PHYSMOVE_SLIDE:
		{
			return 2;
		}

		default:
		{
			TraceLog(LOG_WARNING, "Performing physics movement with unsupported movenent type %d", movementType);
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
	callbackArgs.traceResult = result;

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
	const float deltaLength = Vector2Length(Vector2Scale(physComp->velocity, GetFrameTime()));
	float distanceTravelled = 0.0f;

	for ( size_t iteration = 0; iteration < maxIterations; ++iteration )
	{
		if ( distanceTravelled >= deltaLength )
		{
			break;
		}

		// Do this in two stages, since GetFrameTime() is liable to be small anyway,
		// and we don't want the float factor we're multiplying by to be tiny.
		Vector2 workingDelta = Vector2Scale(physComp->velocity, GetFrameTime());
		workingDelta = Vector2Scale(workingDelta, 1.0f - (distanceTravelled / deltaLength));

		TraceResult result = Physics_TraceHullInWorld(
			world,
			PhysicsComponent_GetWorldCollisionHull(physComp),
			workingDelta,
			physComp->collisionMask,
			PhysicsComponent_GetOwnerEntity(physComp)
		);

		MoveToPosition(physComp, &result);
		FireCollisionCallbacks(physComp, &result, iteration);

		distanceTravelled += result.fraction * Vector2Length(workingDelta);

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
	CollisionMask collisionMask,
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
	struct EntityGroup* group = World_GetDefaultEntityGroup(world);
	for ( Entity* ent = World_GetEntityGroupHead(group); ent; ent = World_GetNextEntity(ent) )
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
