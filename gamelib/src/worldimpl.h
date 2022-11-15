#pragma once

#include "threading.h"
#include "gamelib/world.h"
#include "asyncload/asyncload.h"

struct EntityImpl;
struct WorldImpl;

struct EntityGroup
{
	struct WorldImpl* ownerWorld;

	size_t count;
	struct EntityImpl* head;
	struct EntityImpl* tail;
};

typedef struct WorldImpl
{
	World world;
	EntityGroup defaultEntityGroup;

	sem_t asyncLoadSemaphore;
	pthread_t asyncLoadThread;

	// If present, async load thread exists.
	// If not present, async load thread has been joined.
	AsyncLoadArgs* asyncLoadThreadArgs;

	// TODO: Replace with something more useful
	char asyncMessage[128];
} WorldImpl;
