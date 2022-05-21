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

void SpriteComponentImpl_Render(SpriteComponentImpl* impl)
{
	if ( !impl || !impl->imagePoolItem )
	{
		return;
	}

	Texture2D* texture = ImagePool_GetTexture(impl->imagePoolItem);
	Rectangle source = (Rectangle){ 0.0f, 0.0f, (float)texture->width, (float)texture->height };
	Vector2 pos = impl->ownerEntity->position;
	Vector2 scale = (Vector2){ texture->width * impl->component.scale.x, texture->height * impl->component.scale.y };
	Rectangle dest = (Rectangle){ pos.x, pos.y, scale.x, scale.y };
	Vector2 origin = Vector2Multiply(impl->component.origin, impl->component.scale);

	DrawTexturePro(*texture, source, dest, origin, 0.0f, WHITE);
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
