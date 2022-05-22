#include "entity/spritecomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"
#include "resourcepool.h"
#include "descriptor/spritesheetdescriptor.h"

static void ResetSpriteAnimationVars(SpriteComponentImpl* impl)
{
	impl->animTime = 0.0f;

	if ( impl->sprSheetResource )
	{
		impl->animation = SpriteSheetDescriptor_GetFirstAnimation(ResourcePool_GetSpriteSheet(impl->sprSheetResource));
	}
	else
	{
		impl->animation = NULL;
	}
}

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

void SpriteComponentImpl_Update(SpriteComponentImpl* impl)
{
	if ( !impl )
	{
		return;
	}

	if ( !impl->animation )
	{
		impl->animTime = 0.0f;
		return;
	}

	size_t numFrames = SpriteSheetDescriptor_GetAnimationFrameCount(impl->animation);

	if ( numFrames < 1 )
	{
		impl->animTime = 0.0f;
		return;
	}

	float fps = SpriteSheetDescriptor_GetAnimationFPS(impl->animation);

	// TODO: Put the following into a utility function somewhere?
	// animTime is in the range [0 1), where floor(animTime * numFrames) gives the frame index.
	// If we have an animation of 10 frames running at 3 fps, a deltaTime of 1 implies an
	// increment of 3 frames, which is an animTime increment of 3/10. Therefore the formula becomes:
	impl->animTime += (GetFrameTime() * fps) / (float)numFrames;
	impl->animTime = fmodf(impl->animTime, 1.0f);
}

void SpriteComponentImpl_Render(SpriteComponentImpl* impl)
{
	if ( !impl || !impl->sprSheetResource || !impl->animation )
	{
		return;
	}

	Texture2D* texture = SpriteSheetDescriptor_GetAnimationTexture(impl->animation);
	size_t numFrames = SpriteSheetDescriptor_GetAnimationFrameCount(impl->animation);

	if ( !texture || numFrames < 1 )
	{
		return;
	}

	// Calculate the current frame index from animTime.
	size_t frameIndex = (size_t)(impl->animTime * (float)numFrames);

	// Define the source rect from the texture image that we're going to draw.
	Vector2i sourceRectDim = SpriteSheetDescriptor_GetAnimationFrameBounds(impl->animation);
	Rectangle sourceRect = (Rectangle){ (float)(frameIndex * sourceRectDim.x), 0.0f, (float)sourceRectDim.x, (float)sourceRectDim.y };

	// The scale defines how large the sprite is drawn relative to the source rect's size.
	Vector2 scale = impl->component.scale;

	// The size of the desination rectangle is affected by the scale.
	Vector2 destRectDim = (Vector2){ (float)sourceRectDim.x * scale.x, (float)sourceRectDim.y * scale.y };

	// This is the point on the source rectangle that should be lined up with the entity's position when drawn.
	// The sprite component's offset is applied on top of this later, in world units.
	Vector2 sourceOrigin = SpriteSheetDescriptor_GetOrigin(ResourcePool_GetSpriteSheet(impl->sprSheetResource));

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

	ResourcePoolSpriteSheet* item = ResourcePool_LoadSpriteSheetAndAddRef(filePath);

	if ( item != component->impl->sprSheetResource )
	{
		if ( component->impl->sprSheetResource )
		{
			ResourcePool_RemoveSpriteSheetRef(component->impl->sprSheetResource);
		}

		component->impl->sprSheetResource = item;
		ResetSpriteAnimationVars(component->impl);
	}
	else
	{
		if ( item )
		{
			ResourcePool_RemoveSpriteSheetRef(item);
		}
	}

	return component->impl->sprSheetResource != NULL;
}

bool SpriteComponent_SetAnimationByName(SpriteComponent* component, const char* animName)
{
	if ( !component || !animName || !(*animName) || !component->impl->sprSheetResource )
	{
		return false;
	}

	SpriteSheetDescriptor* sprDesc = ResourcePool_GetSpriteSheet(component->impl->sprSheetResource);
	component->impl->animation = SpriteSheetDescriptor_GetAnimation(sprDesc, animName);
	component->impl->animTime = 0.0f;

	return component->impl->animation != NULL;
}
