#include "entity/entityimpl.h"
#include "gamelib/gameutil.h"
#include "entity/physicscomponentimpl.h"
#include "entity/terraincomponentimpl.h"
#include "gamelib/entity/logiccomponent.h"

static void DestroyAllComponents(EntityImpl* impl)
{
	Entity_DestroyPhysicsComponent(&impl->entity);
	Entity_DestroyTerrainComponent(&impl->entity);
	Entity_DestroyLogicComponent(&impl->entity);
}

void EntityImpl_Destroy(EntityImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	LogicComponent* component = (LogicComponent*)impl->components[COMPONENT_LOGIC];

	if ( component && component->onEntityDestroyed )
	{
		component->onEntityDestroyed(&impl->entity);
	}

	DestroyAllComponents(impl);
	MemFree(impl);
}

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent)
{
	return (ent && ent->impl->components[COMPONENT_PHYSICS])
		? &((PhysicsComponentImpl*)ent->impl->components[COMPONENT_PHYSICS])->component
		: NULL;
}

struct PhysicsComponent* Entity_CreatePhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyPhysicsComponent(ent);

	PhysicsComponentImpl* impl = PhysicsComponentImpl_Create(ent);
	ent->impl->components[COMPONENT_PHYSICS] = impl;

	return &impl->component;
}

void Entity_DestroyPhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	PhysicsComponentImpl_Destroy(ent->impl->components[COMPONENT_PHYSICS]);
	ent->impl->components[COMPONENT_PHYSICS] = NULL;
}

struct TerrainComponent* Entity_GetTerrainComponent(Entity* ent)
{
	return ent ? (TerrainComponent*)ent->impl->components[COMPONENT_TERRAIN] : NULL;
}

struct TerrainComponent* Entity_CreateTerrainComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyTerrainComponent(ent);

	TerrainComponentImpl* impl = TerrainComponentImpl_Create(ent);
	ent->impl->components[COMPONENT_TERRAIN] = impl;

	return &impl->component;
}

void Entity_DestroyTerrainComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	TerrainComponentImpl_Destroy(ent->impl->components[COMPONENT_TERRAIN]);
	ent->impl->components[COMPONENT_TERRAIN] = NULL;
}

struct LogicComponent* Entity_GetLogicComponent(Entity* ent)
{
	return ent ? (LogicComponent*)ent->impl->components[COMPONENT_LOGIC] : NULL;
}

struct LogicComponent* Entity_CreateLogicComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyLogicComponent(ent);

	LogicComponent* component = (LogicComponent*)MemAlloc(sizeof(LogicComponent));
	ent->impl->components[COMPONENT_LOGIC] = component;

	component->ownerEntity = &ent->impl->entity;

	return component;
}

void Entity_DestroyLogicComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	LogicComponent* component = (LogicComponent*)ent->impl->components[COMPONENT_LOGIC];

	if ( component )
	{
		if ( component->onComponentRemoved )
		{
			component->onComponentRemoved(component);
		}

		MemFree(component);
		ent->impl->components[COMPONENT_LOGIC] = NULL;
	}
}
