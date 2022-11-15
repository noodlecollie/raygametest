#pragma once

#include "threading.h"
#include "gamelib/world.h"
#include "asyncload/asyncload.h"
#include "entity/entitygroup.h"

struct EntityImpl;
struct WorldImpl;

typedef struct WorldImpl
{
	World world;
	EntityGroup* defaultEntityGroup;

	sem_t asyncLoadSemaphore;
	pthread_t asyncLoadThread;

	// If present, async load thread exists.
	// If not present, async load thread has been joined.
	AsyncLoadArgs* asyncLoadThreadArgs;

	EntityGroup* asyncLoadedEntityGroup;
} WorldImpl;
