#pragma once

#include "gamelib/entity/spritecomponent.h"
#include "imagepool.h"

struct Entity;

typedef struct SpriteComponentImpl
{
	struct Entity* ownerEntity;
	SpriteComponent component;
	ImagePoolItem* imagePoolItem;
} SpriteComponentImpl;

SpriteComponentImpl* SpriteComponentImpl_Create(struct Entity* ownerEntity);
void SpriteComponentImpl_Destroy(SpriteComponentImpl* impl);
void SpriteComponentImpl_Render(SpriteComponentImpl* impl);
