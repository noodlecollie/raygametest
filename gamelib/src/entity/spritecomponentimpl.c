#include "entity/spritecomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"
#include "resourcepool.h"
#include "descriptor/spritesheetdescriptor.h"

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

	if ( impl->sprSheetResource )
	{
		ResourcePool_RemoveSpriteSheetRef(impl->sprSheetResource);
		impl->sprSheetResource = NULL;
	}

	MemFree(impl);
}

void SpriteComponentImpl_Render(SpriteComponentImpl* impl)
{
	if ( !impl || !impl->sprSheetResource )
	{
		return;
	}

	SpriteSheetDescriptor* sprDesc = ResourcePool_GetSpriteSheet(impl->sprSheetResource);
	ResourcePoolTexture* rpTex = SpriteSheetDescriptor_GetFrame(sprDesc);
	Texture2D* texture = ResourcePool_GetTexture(rpTex);
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

bool SpriteComponent_SetSpriteSheet(SpriteComponent* component, const char* filePath)
{
	if ( !component || !filePath || !(*filePath) )
	{
		return false;
	}

	// Acquire the new resource before releasing the old one.
	// This means that, if the resource that's being set is the same,
	// we avoid unnecessarily unloading and reloading resources.
	ResourcePoolSpriteSheet* item = ResourcePool_LoadSpriteSheetAndAddRef(filePath);

	if ( component->impl->sprSheetResource )
	{
		ResourcePool_RemoveSpriteSheetRef(component->impl->sprSheetResource);
	}

	component->impl->sprSheetResource = item;
	return component->impl->sprSheetResource != NULL;
}
