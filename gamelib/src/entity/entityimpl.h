#pragma once

#include "gamelib/entity/Entity.h"
#include "gamelib/entity/physicscomponent.h"
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

PhysicsComponent* EntityImpl_GetPhysicsComponent(EntityImpl* impl);
PhysicsComponent* EntityImpl_AddPhysicsComponent(EntityImpl* impl);
void EntityImpl_RemovePhysicsComponent(EntityImpl* impl);
