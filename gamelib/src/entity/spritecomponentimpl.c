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

	// TODO: At some point we need to implement animations. For now, display the first frame of the first animation.
	SpriteSheetDescriptor* sprDesc = ResourcePool_GetSpriteSheet(impl->sprSheetResource);
	SpriteSheetAnimation* anim = SpriteSheetDescriptor_GetFirstAnimation(sprDesc);
	Texture2D* texture = SpriteSheetDescriptor_GetAnimationTexture(anim);
	size_t numFrames = SpriteSheetDescriptor_GetAnimationFrameCount(anim);

	if ( !texture || numFrames < 1 )
	{
		return;
	}

	// Define the source rect from the texture image that we're going to draw.
	Vector2i sourceRectDim = SpriteSheetDescriptor_GetAnimationFrameBounds(anim);
	Rectangle sourceRect = (Rectangle){ 0.0f, 0.0f, (float)sourceRectDim.x, (float)sourceRectDim.y };

	// The scale defines how large the sprite is drawn relative to the source rect's size.
	Vector2 scale = impl->component.scale;

	// The size of the desination rectangle is affected by the scale.
	Vector2 destRectDim = (Vector2){ (float)sourceRectDim.x * scale.x, (float)sourceRectDim.y * scale.y };

	// This is the point on the source rectangle that should be lined up with the entity's position when drawn.
	// The sprite component's offset is applied on top of this later, in world units.
	Vector2 sourceOrigin = SpriteSheetDescriptor_GetOrigin(sprDesc);

	// This is where the entity itself is located.
	Vector2 worldPos = impl->ownerEntity->position;

	// The dest rect position must be adjusted to cater for the source origin.
	// The scale of the sprite affects this adjustment.
	Vector2 destRectPos = Vector2Subtract(worldPos, Vector2Multiply(sourceOrigin, scale));

	// The position is further adjusted by the offset set on the component.
	destRectPos = Vector2Add(destRectPos, impl->component.offset);

	// Now we can construct the dest rectangle.
	Rectangle destRect = (Rectangle){ destRectPos.x, destRectPos.y, destRectDim.x, destRectDim.y };

	// It's easier to explicitly do the above than to provide a value for the
	// "origin" argument to this function.
	DrawTexturePro(*texture, sourceRect, destRect, Vector2Zero(), 0.0f, WHITE);
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
