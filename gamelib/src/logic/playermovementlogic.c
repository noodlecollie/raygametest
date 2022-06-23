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
#define PLAYER_MOVEMENT_VEL_SCALE (Vector2){ 200.0f, 450.0f }
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

static bool CheckActivateZipJump(PlayerMovementLogicData* data, Entity* ent)
{
	if ( !IsKeyPressed(KEY_UP) || data->onGround || fabsf(data->inputDir.x) < FLT_EPSILON )
	{
		return false;
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

	return result.collided && !result.beganColliding && !SurfaceNormalIsGround(result.contactNormal);
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

		if ( CheckActivateZipJump(data, ownerEnt) )
		{
			data->lastZipJumpTime = currentTime;
		}

		data->inputDir = Vector2Multiply(data->inputDir, PLAYER_MOVEMENT_VEL_SCALE);

		if ( data->lastZipJumpTime > 0.0f )
		{
			double elapsed = currentTime - data->lastZipJumpTime;

			// TODO: Make this only apply on a wall jump
			if ( elapsed >= 0.0f && elapsed < data->zipDuration )
			{
				data->inputDir.x += Parametric_SinePeak(elapsed / data->zipDuration) * data->zipVelocityMultiplier * data->inputDir.x;
			}
		}

		playerPhys->velocity.x = data->inputDir.x;
		playerPhys->velocity.y += data->inputDir.y;
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

	component->userData = MemAlloc(sizeof(PlayerMovementLogicData));
	component->userDataType = PLAYER_LOGIC_DATA_TYPE_ID;

	PlayerMovementLogicData* data = (PlayerMovementLogicData*)component->userData;
	data->zipVelocityMultiplier = 3.0f;
	data->zipDuration = 0.2f;
}

PlayerMovementLogicData* PlayerMovementLogic_GetDataFromComponent(LogicComponent* component)
{
	return (component && component->userDataType == PLAYER_LOGIC_DATA_TYPE_ID) ? (PlayerMovementLogicData*)component->userData : NULL;
}
