#pragma once

#include "gamelib/entity/spritecomponent.h"
#include "resourcepool/resourcepool.h"
#include "descriptor/spritesheetdescriptor.h"

struct Entity;

typedef struct SpriteComponentImpl
{
	struct Entity* ownerEntity;
	SpriteComponent component;

	ResourcePoolSpriteSheet* sprSheetResource;
	SpriteSheetAnimation* animation;
	float animTime;
} SpriteComponentImpl;

SpriteComponentImpl* SpriteComponentImpl_Create(struct Entity* ownerEntity);
void SpriteComponentImpl_Destroy(SpriteComponentImpl* impl);
void SpriteComponentImpl_Update(SpriteComponentImpl* impl);
void SpriteComponentImpl_Render(SpriteComponentImpl* impl, Camera3D camera);
