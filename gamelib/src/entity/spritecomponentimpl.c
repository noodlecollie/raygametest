#include "entity/spritecomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"
#include "imagepool.h"

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

	if ( impl->imagePoolItem )
	{
		ImagePool_RemoveRef(impl->imagePoolItem);
		impl->imagePoolItem = NULL;
	}

	MemFree(impl);
}

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component)
{
	return component ? component->impl->ownerEntity : NULL;
}

bool SpriteComponent_SetImage(SpriteComponent* component, const char* filePath)
{
	if ( !component || !filePath || !(*filePath) )
	{
		return false;
	}

	// Acquire the new image before releasing the old one.
	// This means that, if the image that's being set is the same,
	// we avoid unnecessarily unloading and reloading resources.
	ImagePoolItem* item = ImagePool_AddRef(filePath);

	if ( component->impl->imagePoolItem )
	{
		ImagePool_RemoveRef(component->impl->imagePoolItem);
	}

	component->impl->imagePoolItem = item;
	return component->impl->imagePoolItem != NULL;
}
