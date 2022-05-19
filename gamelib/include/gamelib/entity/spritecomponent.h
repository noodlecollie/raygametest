#pragma once

#include "gamelib/external/raylibheaders.h"

struct Entity;
struct SpriteComponentImpl;

typedef struct SpriteComponent
{
	struct SpriteComponentImpl* impl;

	Vector2 scale;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
