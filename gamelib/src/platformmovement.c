#include "gamelib/platformmovement.h"
#include "gamelib/trace.h"
#include "gamelib/gameutil.h"
#include "raymath.h"

#define CONTACT_ADJUST_DIST 0.01f

void PlatformMovement_MovePlayer(Player* player, float deltaTime, PlatformerLevel level, uint32_t collisionLayers)
{
	if ( !player )
	{
		return;
	}

	Vector2 delta = Vector2Scale(player->velocity, deltaTime);
	Rectangle hull = Player_GetWorldCollisionHull(player);
	TraceResult result = TraceRectangleMovementInLevel(hull, delta, level, collisionLayers);

	if ( !result.collided )
	{
		player->onGround = false;
		player->position = Vector2Add(player->position, delta);
		return;
	}

	hull.x = result.contactPosition.x;
	hull.y = result.contactPosition.y;

	player->position = Vector2Add(RectangleMid(hull), Vector2Scale(result.contactNormal, CONTACT_ADJUST_DIST));
	player->onGround = result.endedColliding || Vector2DotProduct(result.contactNormal, (Vector2){ 0.0f, -1.0f }) > 0.5f;
}
