#include "gamelib/player.h"

Rectangle Player_GetWorldCollisionHull(const Player* player)
{
	Rectangle hull = { 0 };

	if ( !player )
	{
		return hull;
	}

	hull.x = player->position.x + player->collisionHull.x;
	hull.y = player->position.y + player->collisionHull.y;
	hull.width = player->collisionHull.width;
	hull.height = player->collisionHull.height;

	return hull;
}
