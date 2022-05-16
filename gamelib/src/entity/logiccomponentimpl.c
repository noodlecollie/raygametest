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

struct Entity* LogicComponent_GetOwnerEntity(LogicComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}
