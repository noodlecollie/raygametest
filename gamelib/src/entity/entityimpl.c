#include "entity/entityimpl.h"
#include "gamelib/gameutil.h"

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

	if ( impl->logicImpl && impl->logicImpl->component.onEntityDestroyed )
	{
		impl->logicImpl->component.onEntityDestroyed(&impl->entity);
	}

	DestroyAllComponents(impl);
	MemFree(impl);
}

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent)
{
	return (ent && ent->impl->physicsImpl)
		? &((PhysicsComponentImpl*)ent->impl->physicsImpl)->component
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
	ent->impl->physicsImpl = impl;

	return &impl->component;
}

void Entity_DestroyPhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	PhysicsComponentImpl_Destroy(ent->impl->physicsImpl);
	ent->impl->physicsImpl = NULL;
}

struct TerrainComponent* Entity_GetTerrainComponent(Entity* ent)
{
	return (ent && ent->impl->terrainImpl)
		? &((TerrainComponentImpl*)ent->impl->terrainImpl)->component
		: NULL;
}

struct TerrainComponent* Entity_CreateTerrainComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyTerrainComponent(ent);

	TerrainComponentImpl* impl = TerrainComponentImpl_Create(ent);
	ent->impl->terrainImpl = impl;

	return &impl->component;
}

void Entity_DestroyTerrainComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	TerrainComponentImpl_Destroy(ent->impl->terrainImpl);
	ent->impl->terrainImpl = NULL;
}

struct LogicComponent* Entity_GetLogicComponent(Entity* ent)
{
	return (ent && ent->impl->logicImpl)
		? &((LogicComponentImpl*)ent->impl->logicImpl)->component
		: NULL;
}

struct LogicComponent* Entity_CreateLogicComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyLogicComponent(ent);

	LogicComponentImpl* impl = LogicComponentImpl_Create(ent);
	ent->impl->logicImpl = impl;

	return &impl->component;
}

void Entity_DestroyLogicComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	if ( ent->impl->logicImpl )
	{
		if ( ent->impl->logicImpl->component.onComponentRemoved )
		{
			ent->impl->logicImpl->component.onComponentRemoved(&ent->impl->logicImpl->component);
		}

		LogicComponentImpl_Destroy(ent->impl->logicImpl);
		ent->impl->logicImpl = NULL;
	}
}
