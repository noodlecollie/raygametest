#pragma once

#include "gamelib/entity/entity.h"
#include "entity/componenttypes.h"

struct World;

typedef struct EntityImpl
{
	struct World* ownerWorld;
	struct EntityImpl* prev;
	struct EntityImpl* next;

	Entity entity;
	void* components[COMPONENT_TOTAL_TYPES];
} EntityImpl;

void EntityImpl_Destroy(EntityImpl* impl);
