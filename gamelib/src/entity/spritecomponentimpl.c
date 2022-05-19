#include "entity/spritecomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"

SpriteComponentImpl* SpriteComponentImpl_Create(struct Entity* ownerEntity)
{
	if ( !ownerEntity )
	{
		return NULL;
	}

	SpriteComponentImpl* impl = (SpriteComponentImpl*)MemAlloc(sizeof(SpriteComponentImpl));

	impl->component.impl = impl;
	impl->ownerEntity = ownerEntity;
	impl->component.scale = (Vector2){ 1.0f, 1.0f };

	return impl;
}

void SpriteComponentImpl_Destroy(SpriteComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	MemFree(impl);
}

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}
