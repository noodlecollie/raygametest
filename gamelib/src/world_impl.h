#pragma once

struct EntityImpl;

typedef struct EntityGroup
{
	size_t count;
	struct EntityImpl* head;
	struct EntityImpl* tail;
} EntityGroup;

typedef struct WorldImpl
{
	EntityGroup defaultEntityGroup;
} WorldImpl;
