#include "entity/logiccomponentimpl.h"
#include "gamelib/entity/entity.h"

LogicComponentImpl* LogicComponentImpl_Create(struct Entity* ownerEntity)
{
	if ( !ownerEntity )
	{
		return NULL;
	}

	LogicComponentImpl* impl = (LogicComponentImpl*)MemAlloc(sizeof(LogicComponentImpl));

	impl->component.impl = impl;
	impl->ownerEntity = ownerEntity;

	return impl;
}

void LogicComponentImpl_Destroy(LogicComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	MemFree(impl);
}

struct Entity* LogicComponent_GetOwnerEntity(const LogicComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}

void LogicComponent_PerformCleanup(LogicComponent* component)
{
	if ( !component )
	{
		return;
	}

	if ( component->callbacks.onComponentCleanup )
	{
		component->callbacks.onComponentCleanup(component);
	}

	component->callbacks = (LogicComponentCallbacks){ 0 };
	component->userData = NULL;
	component->userDataType = 0;
}
