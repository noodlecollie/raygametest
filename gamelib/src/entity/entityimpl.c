#include "entity/entityimpl.h"
#include "gamelib/gameutil.h"
#include "gamelib/entity/physicscomponent.h"
#include "gamelib/entity/terraincomponent.h"

#define ALLOCATE_COMPONENT(impl, type, componentType) ((type*)AllocateComponent(impl, componentType, sizeof(type)))

static inline void DestroyComponent(EntityImpl* impl, ComponentType componentType)
{
	if ( impl->components[componentType] )
	{
		MemFree(impl->components[componentType]);
		impl->components[componentType] = NULL;
	}
}

static inline void* AllocateComponent(EntityImpl* impl, ComponentType componentType, size_t structSize)
{
	void** component = &impl->components[componentType];

	if ( *component )
	{
		DestroyComponent(impl, componentType);
	}

	*component = MemAlloc((int)structSize);

	return *component;
}

static void DestroyAllComponents(EntityImpl* impl)
{
	for ( size_t index = 0; index < ARRAY_SIZE(impl->components); ++index )
	{
		if ( impl->components[index] )
		{
			MemFree(impl->components[index]);
			impl->components[index] = NULL;
		}
	}
}

void EntityImpl_Destroy(EntityImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	DestroyAllComponents(impl);
	MemFree(impl);
}

struct PhysicsComponent* EntityImpl_GetPhysicsComponent(Entity* ent)
{
	return ent ? (PhysicsComponent*)ent->impl->components[COMPONENT_PHYSICS] : NULL;
}

struct PhysicsComponent* EntityImpl_AddPhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	PhysicsComponent* component = ALLOCATE_COMPONENT(ent->impl, PhysicsComponent, COMPONENT_PHYSICS);

	component->ownerEntity = &ent->impl->entity;
	component->gravityModifier = 1.0f;

	return component;
}

void EntityImpl_RemovePhysicsComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	DestroyComponent(ent->impl, COMPONENT_PHYSICS);
}

struct TerrainComponent* Entity_GetTerraimComponent(Entity* ent)
{
	return ent ? (TerrainComponent*)ent->impl->components[COMPONENT_TERRAIN] : NULL;
}

struct TerrainComponent* Entity_AddTerrainComponent(Entity* ent)
{
	if ( !ent || !ent->impl )
	{
		return NULL;
	}

	TerrainComponent* component = ALLOCATE_COMPONENT(ent->impl, TerrainComponent, COMPONENT_TERRAIN);

	component->ownerEntity = &ent->impl->entity;
	component->scale = 1.0f;

	return component;
}

void Entity_RemoveTerrainComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	DestroyComponent(ent->impl, COMPONENT_TERRAIN);
}
