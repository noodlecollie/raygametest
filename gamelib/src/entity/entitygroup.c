#include "entity/entitygroup.h"
#include "entity/entityimpl.h"

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
