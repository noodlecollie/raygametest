#include "gamelib/oldplayer.h"

OldPlayer OldPlayer_Create(void)
{
	OldPlayer player = { 0 };

	player.entity = OldEntity_Create();

	return player;
}

void OldPlayer_Destroy(OldPlayer* player)
{
	if ( !player )
	{
		return;
	}

	OldEntity_Destroy(player->entity);
}
