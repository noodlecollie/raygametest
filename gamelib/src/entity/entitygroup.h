#pragma once

#include <stddef.h>

struct WorldImpl;
struct EntityImpl;

typedef struct EntityGroup
{
	struct WorldImpl* ownerWorld;

	struct EntityGroup* groupHead;
	struct EntityGroup* groupTail;

	size_t count;
	struct EntityImpl* head;
	struct EntityImpl* tail;
} EntityGroup;

EntityGroup* EntityGroup_Create(struct WorldImpl* ownerWorld);
void EntityGroup_Destroy(EntityGroup* group);
void EntityGroup_DestroyAllEntities(EntityGroup* group);

struct Entity* EntityGroup_CreateEntity(EntityGroup* group);
