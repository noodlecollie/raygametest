#include "entity/entitygroup.h"
#include "entity/entityimpl.h"
#include "listmacros.h"

EntityGroup* EntityGroup_Create(struct WorldImpl* ownerWorld)
{
	EntityGroup* group = (EntityGroup*)MemAlloc(sizeof(EntityGroup));

	group->ownerWorld = ownerWorld;

	return group;
}

void EntityGroup_Destroy(EntityGroup* group)
{
	if ( !group )
	{
		return;
	}

	EntityGroup_DestroyAllEntities(group);
	MemFree(group);
}

void EntityGroup_DestroyAllEntities(EntityGroup* group)
{
	if ( !group )
	{
		return;
	}

	EntityImpl* slot = group->head;

	group->head = NULL;
	group->tail = NULL;
	group->count = 0;

	while ( slot )
	{
		EntityImpl* next = slot->next;
		EntityImpl_Destroy(slot);
		slot = next;
	}
}

struct Entity* EntityGroup_CreateEntity(EntityGroup* group)
{
	if ( !group )
	{
		return NULL;
	}

	EntityImpl* impl = (EntityImpl*)MemAlloc(sizeof(EntityImpl));

	impl->ownerGroup = group;
	impl->entity.impl = impl;

	DBL_LL_ADD_TO_TAIL(impl, prev, next, group, head, tail);

	++group->count;

	return &impl->entity;
}
