#pragma once

#include "gamelib/world.h"

struct EntityImpl;
struct WorldImpl;

typedef struct EntityGroup
{
	struct WorldImpl* ownerWorld;

	size_t count;
	struct EntityImpl* head;
	struct EntityImpl* tail;
} EntityGroup;

typedef struct WorldImpl
{
	World world;
	EntityGroup defaultEntityGroup;
} WorldImpl;
