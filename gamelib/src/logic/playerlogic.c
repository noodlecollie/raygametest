#include "gamelib/logic/playerlogic.h"
#include "gamelib/external/raylibheaders.h"
#include "gamelib/entity/entity.h"
#include "gamelib/trace.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/terraincomponent.h"
#include "gamelib/physics.h"

#define PLAYER_COLLISION_MASK 0xFFFFFFFF
#define PLAYER_MOVEMENT_VEL_SCALE (Vector2){ 200.0f, 450.0f }

static inline bool SurfaceNormalIsGround(Vector2 normal)
{
	return Vector2DotProduct(normal, (Vector2){ 0.0f, -1.0f }) > 0.5f;
}

static inline bool CheckIfStandingOnGround(PhysicsComponent* physComp, TerrainComponent* terrain)
{
	if ( !physComp || !terrain )
	{
		return false;
	}

	Vector2 delta = (Vector2){ 0.0f, 2.0f * PHYSICS_CONTACT_ADJUST_DIST };
	Rectangle hull = PhysicsComponent_GetWorldCollisionHull(physComp);
	TraceResult result = TraceRectangleMovementAgainstTerrain(hull, delta, terrain, PLAYER_COLLISION_MASK);

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

static void OnPreThink(LogicComponent* component)
{
	PlayerLogic* data = (PlayerLogic*)component->userData;
	Entity* ownerEnt = LogicComponent_GetOwnerEntity(component);
	PhysicsComponent* playerPhys = Entity_GetPhysicsComponent(ownerEnt);

	if ( playerPhys )
	{
		Vector2 movement = Vector2Zero();

		if ( IsKeyDown(KEY_RIGHT) )
		{
			movement.x += 1.0f;
		}

		if ( IsKeyDown(KEY_LEFT) )
		{
			movement.x -= 1.0f;
		}

		if ( IsKeyPressed(KEY_UP) && data->onGround )
		{
			movement.y -= 1.0f;
		}

		movement = Vector2Multiply(movement, PLAYER_MOVEMENT_VEL_SCALE);

		playerPhys->velocity.x = movement.x;
		playerPhys->velocity.y += movement.y;
	}

	// Before physics sim, assume the player will not be on the ground.
	data->onGround = false;
}

static void OnPhysicsCollided(LogicComponent* component, Entity* otherEntity)
{
	PlayerLogic* data = (PlayerLogic*)component->userData;
	Entity* thisEntity = LogicComponent_GetOwnerEntity(component);

	data->onGround = CheckIfStandingOnGround(Entity_GetPhysicsComponent(thisEntity), Entity_GetTerrainComponent(otherEntity));
}

void PlayerLogic_InitComponent(LogicComponent* component)
{
	if ( !component )
	{
		return;
	}

	LogicComponent_PerformCleanup(component);

	component->callbacks.onComponentCleanup = &OnComponentCleanup;
	component->callbacks.onPreThink = &OnPreThink;
	component->callbacks.onPhysicsCollided = &OnPhysicsCollided;

	component->userData = MemAlloc(sizeof(PlayerLogic));
}
