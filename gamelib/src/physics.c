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

static inline void MoveToPositionOld(OldEntity* entity, const TraceResult* result)
{
	OldPhysicsComponent* physComp = OldEntity_PhysicsComponent(entity);
	Rectangle hull = OldPhysicsComponent_GetWorldCollisionHull(physComp);

	entity->position.x += result->endPosition.x - hull.x;
	entity->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		entity->position = Vector2Add(entity->position, Vector2Scale(result->contactNormal, PHYSICS_CONTACT_ADJUST_DIST));
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

static float SimulateAgainstTerrain(PhysicsComponent* physComp, TerrainComponent* terrain, Vector2 delta, TraceResult* outResult)
{
	*outResult = TraceRectangleMovementAgainstTerrain(
		PhysicsComponent_GetWorldCollisionHull(physComp),
		delta,
		terrain,
		physComp->collisionMask
	);

	float traceFraction = outResult->fraction;

	if ( outResult->collided && !outResult->endedColliding )
	{
		// For the remainder of the movement, slide along the surface.
		Vector2 remainderDelta = Vector2Scale(delta, fmaxf(1.0f - outResult->fraction, 0.0f));
		remainderDelta = Vector2ProjectAlongSurface(remainderDelta, outResult->contactNormal);

		*outResult = TraceRectangleMovementAgainstTerrain(
			PhysicsComponent_GetWorldCollisionHull(physComp),
			remainderDelta,
			terrain,
			physComp->collisionMask
		);
	}

	return traceFraction;
}

static void ApplyPerFrameDeltas(const World* world, PhysicsComponent* physComp)
{
	float deltaTime = GetFrameTime();

	// Add in gravity applied over this time delta.
	physComp->velocity.y += world->gravity * physComp->gravityModifier * deltaTime;
}

static void MoveToPosition(PhysicsComponent* physComp, const TraceResult* result)
{
	Entity* ent = physComp->ownerEntity;
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);

	ent->position.x += result->endPosition.x - hull.x;
	ent->position.y += result->endPosition.y - hull.y;

	if ( result->collided )
	{
		ent->position = Vector2Add(ent->position, Vector2Scale(result->contactNormal, PHYSICS_CONTACT_ADJUST_DIST));
		physComp->velocity = (!result->endedColliding) ? Vector2ProjectAlongSurface(physComp->velocity, result->contactNormal) : Vector2Zero();
	}
}

void Physics_SimulateObjectInWorld(struct World* world, struct PhysicsComponent* physComp)
{
	if ( !physComp || !world || !physComp->enabled )
	{
		return;
	}

	ApplyPerFrameDeltas(world, physComp);

	// Get how far we need to now move in this time delta.
	Vector2 origDelta = Vector2Scale(physComp->velocity, GetFrameTime());
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);
	Vector2 hullPos = (Vector2){ hull.x, hull.y };

	// Find the final contact position from the collision with the nearest component.
	float shortestFraction = FLT_MAX;
	TraceResult shortestTraceResult = TraceResultNoCollision(Vector2Add(hullPos, origDelta));
	Entity* collisionEnt = NULL;

	// TODO: This is a naive search. It should be optimised with use of a quadtree.
	for ( Entity* ent = World_GetEntityListHead(world); ent; ent = World_GetNextEntity(ent) )
	{
		if ( Entity_GetTerrainComponent(ent) )
		{
			TraceResult workingResult = TraceResultNoCollision(Vector2Add(hullPos, origDelta));
			float fraction = SimulateAgainstTerrain(physComp, Entity_GetTerrainComponent(ent), origDelta, &workingResult);

			if ( fraction < shortestFraction )
			{
				shortestFraction = fraction;
				shortestTraceResult = workingResult;
				collisionEnt = ent;
			}
		}
	}

	MoveToPosition(physComp, &shortestTraceResult);

	if ( collisionEnt )
	{
		LogicComponent* logic = Entity_GetLogicComponent(physComp->ownerEntity);

		if ( logic && logic->onPhysicsCollided )
		{
			logic->onPhysicsCollided(physComp->ownerEntity, collisionEnt);
		}

		logic = Entity_GetLogicComponent(collisionEnt);

		if ( logic && logic->onPhysicsCollided )
		{
			logic->onPhysicsCollided(collisionEnt, physComp->ownerEntity);
		}
	}
}
