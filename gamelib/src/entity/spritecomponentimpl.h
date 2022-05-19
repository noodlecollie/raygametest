#pragma once

#include "gamelib/entity/spritecomponent.h"

struct Entity;

typedef struct SpriteComponentImpl
{
	struct Entity* ownerEntity;
	SpriteComponent component;
} SpriteComponentImpl;

SpriteComponentImpl* SpriteComponentImpl_Create(struct Entity* ownerEntity);
void SpriteComponentImpl_Destroy(SpriteComponentImpl* impl);
