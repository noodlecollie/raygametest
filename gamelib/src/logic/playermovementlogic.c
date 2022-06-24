#include <float.h>
#include "gamelib/logic/playermovementlogic.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/entity/entity.h"
#include "gamelib/trace.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/entity/spritecomponent.h"
#include "gamelib/physics.h"
#include "gamelib/world.h"
#include "gamelib/parametric.h"

#define PLAYER_LOGIC_DATA_TYPE_ID 12345
#define WALLJUMP_CHECK_DIST 8.0f

static inline bool SurfaceNormalIsGround(Vector2 normal)
{
	return Vector2DotProduct(normal, (Vector2){ 0.0f, -1.0f }) > 0.5f;
}

static inline bool CheckIfStandingOnGround(Entity* entity)
{
	if ( !entity )
	{
		return false;
	}

	PhysicsComponent* physComp = Entity_GetPhysicsComponent(entity);
	World* world = Entity_GetWorld(entity);

	if ( !physComp || !world )
	{
		return false;
	}

	Vector2 delta = (Vector2){ 0.0f, 2.0f * PHYSICS_CONTACT_ADJUST_DIST };

	TraceResult result = Physics_TraceHullInWorld(
		world,
		PhysicsComponent_GetWorldCollisionHull(physComp),
		delta,
		physComp->collisionMask,
		entity
	);

	return result.collided && SurfaceNormalIsGround(result.contactNormal);
}

static void OnComponentCleanup(LogicComponent* component)
{
	if ( component->userData )
	{
		MemFree(component->userData);
		component->userData = NULL;
	}
}

static void CheckActivateZipJump(PlayerMovementLogicData* data, Entity* ent, double currentTime)
{
	data->activatedZipJump = false;

	if ( !IsKeyDown(KEY_UP) ||
	     data->onGround ||
	     fabsf(data->inputDir.x) < FLT_EPSILON ||
	     currentTime - data->lastZipJumpTime < data->zipDuration )
	{
		return;
	}

	Vector2 backwards = Vector2Zero();
	backwards.x = (data->inputDir.x < 0.0f ? 1.0f : -1.0f) * WALLJUMP_CHECK_DIST;

	TraceResult result = Physics_TraceHullInWorld(
		Entity_GetWorld(ent),
		PhysicsComponent_GetWorldCollisionHull(Entity_GetPhysicsComponent(ent)),
		backwards,
		1, // TODO: Define collision layers properly - do they belong to physics or to trace?
		ent
	);

	data->activatedZipJump = result.collided && !result.beganColliding && !SurfaceNormalIsGround(result.contactNormal);

	if ( data->activatedZipJump )
	{
		data->lastZipJumpTime = currentTime;
	}
}

static void ApplyZipJump(PlayerMovementLogicData* data, double currentTime)
{
	if ( data->lastZipJumpTime <= 0.0f )
	{
		return;
	}

	double elapsed = currentTime - data->lastZipJumpTime;

	if ( elapsed >= 0.0f && elapsed < data->zipDuration )
	{
		data->wishVel.x *= 1.0f + (Parametric_SinePeak(elapsed / data->zipDuration) * data->zipVelocityMultiplier);
	}

	if ( data->activatedZipJump )
	{
		// Activated this frame, so add a vertical velocity bump too.
		data->wishVel.y -= data->zipJumpImpulse;
	}
}

static void OnPreThink(LogicComponent* component)
{
	PlayerMovementLogicData* data = (PlayerMovementLogicData*)component->userData;
	Entity* ownerEnt = LogicComponent_GetOwnerEntity(component);
	PhysicsComponent* playerPhys = Entity_GetPhysicsComponent(ownerEnt);
	const double currentTime = GetTime();

	if ( playerPhys )
	{
		data->inputDir = Vector2Zero();
		data->wishVel = Vector2Zero();

		if ( IsKeyDown(KEY_RIGHT) )
		{
			data->inputDir.x += 1.0f;
		}

		if ( IsKeyDown(KEY_LEFT) )
		{
			data->inputDir.x -= 1.0f;
		}

		if ( IsKeyPressed(KEY_UP) && data->onGround )
		{
			data->inputDir.y -= 1.0f;
		}

		CheckActivateZipJump(data, ownerEnt, currentTime);

		data->wishVel = Vector2Multiply(data->inputDir, (Vector2){ data->horizontalInputSpeed, data->jumpImpulse });

		ApplyZipJump(data, currentTime);

		playerPhys->velocity.x = data->wishVel.x;
		playerPhys->velocity.y += data->wishVel.y;
	}

	// Before physics sim, assume the player will not be on the ground.
	data->onGround = false;
}

static void OnPostThink(LogicComponent* component)
{
	PlayerMovementLogicData* data = (PlayerMovementLogicData*)component->userData;
	Entity* thisEntity = LogicComponent_GetOwnerEntity(component);

	data->onGround = CheckIfStandingOnGround(thisEntity);

	SpriteComponent* playerSprite = Entity_GetSpriteComponent(thisEntity);
	PhysicsComponent* playerPhys = Entity_GetPhysicsComponent(thisEntity);

	if ( playerSprite && playerPhys )
	{
		if ( playerPhys->velocity.x > 0.0f )
		{
			playerSprite->transformFlags &= ~SPRITE_TRANS_FLIP_X;
		}
		else if ( playerPhys->velocity.x < 0.0f )
		{
			playerSprite->transformFlags |= SPRITE_TRANS_FLIP_X;
		}
	}

	if ( data->onGround )
	{
		float speed = Vector2Length(playerPhys->velocity);

		if ( speed < FLT_EPSILON )
		{
			if ( data->animStanding[0] )
			{
				SpriteComponent_SetAnimationByName(playerSprite, data->animStanding);
			}
		}
		else
		{
			if ( data->animRunning[0] )
			{
				SpriteComponent_SetAnimationByName(playerSprite, data->animRunning);
			}
		}
	}
	else
	{
		if ( playerPhys->velocity.y < 0.0f )
		{
			if ( data->animJumping[0] )
			{
				SpriteComponent_SetAnimationByName(playerSprite, data->animJumping);
			}
		}
		else if ( playerPhys->velocity.y > data->fallSpeedAnimThreshold )
		{
			if ( data->animFalling[0] )
			{
				SpriteComponent_SetAnimationByName(playerSprite, data->animFalling);
			}
		}
	}
}

static void OnPhysicsCollided(struct LogicComponent* component, const OnPhysicsCollidedArgs* args)
{
	PlayerMovementLogicData* data = (PlayerMovementLogicData*)component->userData;

	if ( SurfaceNormalIsGround(args->traceResult->contactNormal) )
	{
		// If we hit the ground, stop any zip in progress.
		data->lastZipJumpTime = 0.0f;
	}
}

void PlayerMovementLogic_SetOnComponent(LogicComponent* component)
{
	if ( !component )
	{
		return;
	}

	LogicComponent_PerformCleanup(component);

	component->callbacks.onComponentCleanup = &OnComponentCleanup;
	component->callbacks.onPreThink = &OnPreThink;
	component->callbacks.onPostThink = &OnPostThink;
	component->callbacks.onPhysicsCollided = &OnPhysicsCollided;

	component->userData = MemAlloc(sizeof(PlayerMovementLogicData));
	component->userDataType = PLAYER_LOGIC_DATA_TYPE_ID;

	PlayerMovementLogicData* data = (PlayerMovementLogicData*)component->userData;

	// Defaults:
	data->horizontalInputSpeed = 200.0f;
	data->jumpImpulse = 450.0f;
	data->zipVelocityMultiplier = 3.0f;
	data->zipDuration = 0.2f;
	data->zipJumpImpulse = 200.0f;
}

PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component)
{
	return (component && component->userDataType == PLAYER_LOGIC_DATA_TYPE_ID) ? (PlayerMovementLogicData*)component->userData : NULL;
}
