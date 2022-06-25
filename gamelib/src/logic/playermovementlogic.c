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
#include "gamelib/debugging/debugging.h"
#include "gamelib/debugging/debugrendercustom.h"

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

static inline bool StateAllowsNewZipJump(PlayerMovementLogicData* data)
{
	return
		IsKeyDown(KEY_UP) &&						// We are holding jump
		!data->onGround &&							// We are not on the ground
		data->inJump &&								// We are in a jump (as opposed to simply falling)
		fabsf(data->inputDir.x) > FLT_EPSILON &&	// We have some movement input on X
		data->lastZipJumpTime <= 0.0f				// We are not in the middle of an unterminated zip jump
	;

}

static bool LocationAllowsNewZipJump(PlayerMovementLogicData* data, Entity* ent)
{
	World* world = Entity_GetWorld(ent);
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(Entity_GetPhysicsComponent(ent));

	// Firstly, trace backwards to see if there's a surface to spring off.
	Vector2 backwards = Vector2Zero();
	backwards.x = (data->inputDir.x < 0.0f ? 1.0f : -1.0f) * WALLJUMP_CHECK_DIST;

	TraceResult result = Physics_TraceHullInWorld(
		world,
		hull,
		backwards,
		COLLISIONMASK_TERRAIN,
		ent
	);

	bool surfaceToSpringboardOff = result.collided && !result.beganColliding && !SurfaceNormalIsGround(result.contactNormal);

	// Then also trace downwards. We want the player to have jumped at least a little bit in the
	// air before they can zip.
	Vector2 downwards = (Vector2){ 0.0f, hull.height };

	result = Physics_TraceHullInWorld(
		world,
		hull,
		downwards,
		COLLISIONMASK_TERRAIN,
		ent
	);

	bool tooCloseToGround = result.collided && SurfaceNormalIsGround(result.contactNormal);

	if ( Debugging.debuggingEnabled && Debugging.renderCustomDebugItems )
	{
		DebugRenderCustom_Rectangle(
			(Rectangle){ hull.x + backwards.x, hull.y + backwards.y, hull.width, hull.height },
			surfaceToSpringboardOff ? GREEN : RED,
			1.0f
		);

		DebugRenderCustom_Rectangle(
			(Rectangle){ hull.x + downwards.x, hull.y + downwards.y, hull.width, hull.height },
			tooCloseToGround ? RED : GREEN,
			1.0f
		);
	}

	return surfaceToSpringboardOff && !tooCloseToGround;
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
		float boostFactor = 0.0f;

		// If neither of these passes, no boost is applied.
		if ( fabsf(data->inputDir.x) > FLT_EPSILON )
		{
			boostFactor = data->inputDir.x > 0.0f ? 1.0f : -1.0f;
		}
		else if ( fabsf(data->wishVel.x) > FLT_EPSILON )
		{
			boostFactor = data->wishVel.x > 0.0f ? 1.0f : -1.0f;
		}

		data->wishVel.x += Parametric_SinePeak((float)elapsed / data->zipDuration) * data->zipVelocityBoost * boostFactor;
	}

	if ( data->activatedZipJumpThisFrame )
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

		if ( IsKeyPressed(KEY_UP) && data->onGround && !data->inJump )
		{
			data->inputDir.y -= 1.0f;
			data->inJump = true;
		}

		data->activatedZipJumpThisFrame = false;

		if ( StateAllowsNewZipJump(data) && LocationAllowsNewZipJump(data, ownerEnt) )
		{
			data->activatedZipJumpThisFrame = true;
			data->lastZipJumpTime = currentTime;
		}

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

	if ( data->onGround )
	{
		data->inJump = false;

		// Reset so that we can re-zip.
		data->lastZipJumpTime = 0.0f;
	}

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

	if ( args->recipientComponentType == COMPONENT_TERRAIN )
	{
		// If we hit anything, stop any zip in progress.
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
	data->zipVelocityBoost = 500.0f;
	data->zipDuration = 0.2f;
	data->zipJumpImpulse = 200.0f;
}

PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component)
{
	return (component && component->userDataType == PLAYER_LOGIC_DATA_TYPE_ID) ? (PlayerMovementLogicData*)component->userData : NULL;
}
