#include "entity/entityimpl.h"
#include "gamelib/gameutil.h"
#include "listmacros.h"

static void DestroyAllLogicComponents(EntityImpl* impl)
{
	LogicComponentImpl* logic = impl->logicImplHead;
	impl->logicImplHead = NULL;
	impl->logicImplTail = NULL;

	while ( logic )
	{
		LogicComponentImpl* next = logic->next;
		LogicComponentImpl_Destroy(logic);
		logic = next;
	}
}

static void DestroyAllComponents(EntityImpl* impl)
{
	Entity_DestroyPhysicsComponent(&impl->entity);
	Entity_DestroyTerrainComponent(&impl->entity);
	Entity_DestroySpriteComponent(&impl->entity);
	Entity_DestroyCameraComponent(&impl->entity);
	DestroyAllLogicComponents(impl);
}

void EntityImpl_Destroy(EntityImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	for ( LogicComponentImpl* logic = impl->logicImplHead; logic; logic = logic->next )
	{
		if ( logic->component.callbacks.onEntityDestroyed )
		{
			logic->component.callbacks.onEntityDestroyed(&logic->component);
		}
	}

	for ( LogicComponentImpl* logic = impl->logicImplHead; logic; logic = logic->next )
	{
		LogicComponent_PerformCleanup(&logic->component);
	}

	DestroyAllComponents(impl);
	MemFree(impl);
}

void EntityImpl_Render(EntityImpl* impl, Camera2D camera)
{
	if ( !impl )
	{
		return;
	}

	if ( impl->terrainImpl )
	{
		TerrainComponentImpl_Render(impl->terrainImpl, camera);
	}

	if ( impl->spriteImpl )
	{
		SpriteComponentImpl_Render(impl->spriteImpl, camera);
	}
}

struct World* Entity_GetWorld(Entity* ent)
{
	return ent ? ent->impl->ownerWorld : NULL;
}

struct PhysicsComponent* Entity_GetPhysicsComponent(Entity* ent)
{
	return (ent && ent->impl->physicsImpl)
		? &ent->impl->physicsImpl->component
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
		? &ent->impl->terrainImpl->component
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

struct SpriteComponent* Entity_GetSpriteComponent(Entity* ent)
{
	return (ent && ent->impl->spriteImpl)
		? &ent->impl->spriteImpl->component
		: NULL;
}

struct SpriteComponent* Entity_CreateSpriteComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroySpriteComponent(ent);

	SpriteComponentImpl* impl = SpriteComponentImpl_Create(ent);
	ent->impl->spriteImpl = impl;

	return &impl->component;
}

void Entity_DestroySpriteComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	SpriteComponentImpl_Destroy(ent->impl->spriteImpl);
	ent->impl->spriteImpl = NULL;
}

struct CameraComponent* Entity_GetCameraComponent(Entity* ent)
{
	return (ent && ent->impl->cameraImpl)
		? &ent->impl->cameraImpl->component
		: NULL;
}

struct CameraComponent* Entity_CreateCameraComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	Entity_DestroyCameraComponent(ent);

	CameraComponentImpl* impl = CameraComponentImpl_Create(ent);
	ent->impl->cameraImpl = impl;

	return &impl->component;
}

void Entity_DestroyCameraComponent(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	CameraComponentImpl_Destroy(ent->impl->cameraImpl);
	ent->impl->cameraImpl = NULL;
}

struct LogicComponent* Entity_GetLogicComponentListHead(Entity* ent)
{
	return (ent && ent->impl->logicImplHead)
		? &ent->impl->logicImplHead->component
		: NULL;
}

struct LogicComponent* Entity_AddLogicComponent(Entity* ent)
{
	if ( !ent )
	{
		return NULL;
	}

	LogicComponentImpl* impl = LogicComponentImpl_Create(ent);

	DBL_LL_ADD_TO_TAIL(impl, prev, next, ent->impl, logicImplHead, logicImplTail);

	return &impl->component;
}

void Entity_RemoveLogicComponent(struct LogicComponent* component)
{
	if ( !component )
	{
		return;
	}

	LogicComponent_PerformCleanup(component);

	LogicComponentImpl* impl = component->impl;
	Entity* ownerEnt = impl->ownerEntity;

	DBL_LL_REMOVE(impl, prev, next, ownerEnt->impl, logicImplHead, logicImplTail);
}

void Entity_RemoveAllLogicComponents(Entity* ent)
{
	if ( !ent )
	{
		return;
	}

	for ( LogicComponentImpl* logic = ent->impl->logicImplHead; logic; logic = logic->next )
	{
		LogicComponent_PerformCleanup(&logic->component);
	}

	DestroyAllLogicComponents(ent->impl);
}

struct LogicComponent* Entity_GetPreviousLogicComponent(struct LogicComponent* component)
{
	return (component && component->impl->prev)
		? &component->impl->prev->component
		: NULL;
}

struct LogicComponent* Entity_GetNextLogicComponent(struct LogicComponent* component)
{
	return (component && component->impl->next)
		? &component->impl->next->component
		: NULL;
}
