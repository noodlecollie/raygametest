#include "gamelib/player.h"

Player Player_Create(void)
{
	Player player = { 0 };

	player.entity = Entity_Create();

	return player;
}

void Player_Destroy(Player* player)
{
	if ( !player )
	{
		return;
	}

	Entity_Destroy(player->entity);
}
