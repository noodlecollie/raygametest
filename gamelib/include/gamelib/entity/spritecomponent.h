#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"

struct Entity;
struct SpriteComponentImpl;

typedef struct SpriteComponent
{
	struct SpriteComponentImpl* impl;

	Vector2 scale;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
bool SpriteComponent_SetImage(SpriteComponent* component, const char* filePath);
