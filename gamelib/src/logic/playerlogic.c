#include "gamelib/logic/playerlogic.h"
#include "gamelib/external/raylibheaders.h"

static void OnComponentCleanup(LogicComponent* component)
{
	if ( component->userData )
	{
		MemFree(component->userData);
		component->userData = NULL;
	}
}

void PlayerLogic_InitComponent(LogicComponent* component)
{
	if ( !component )
	{
		return;
	}

	LogicComponent_PerformCleanup(component);

	component->callbacks.onComponentCleanup = &OnComponentCleanup;
	component->userData = MemAlloc(sizeof(PlayerLogic));
}
