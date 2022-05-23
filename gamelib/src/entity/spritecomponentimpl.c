#include "entity/spritecomponentimpl.h"
#include "gamelib/entity/entity.h"
#include "gamelib/external/raylibheaders.h"
#include "resourcepool.h"
#include "descriptor/spritesheetdescriptor.h"
#include "rendering/spriterenderer.h"

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
	if ( !impl || !impl->animation )
	{
		return;
	}

	size_t numFrames = SpriteSheetDescriptor_GetAnimationFrameCount(impl->animation);

	if ( numFrames < 1 )
	{
		return;
	}

	SpriteRenderer_DrawSpriteFrame(
		impl->animation,
		(size_t)(impl->animTime * (float)numFrames),
		Vector2Add(impl->ownerEntity->position, impl->component.offset),
		impl->component.scale
	);
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
