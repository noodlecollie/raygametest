#pragma once

#include "gamelib/world.h"
#include "entity/entitygroup.h"

struct EntityImpl;
struct WorldImpl;

typedef struct WorldImpl
{
	World world;
	EntityGroup* defaultEntityGroup;
} WorldImpl;
