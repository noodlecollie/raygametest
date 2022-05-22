#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"

struct Entity;
struct SpriteComponentImpl;

typedef struct SpriteComponent
{
	struct SpriteComponentImpl* impl;

	// Sprite origin offset from the entity's position.
	Vector2 offset;

	// How big the sprite should be in X and Y,
	// in units per pixel.
	Vector2 scale;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
bool SpriteComponent_SetSpriteSheet(SpriteComponent* component, const char* filePath);
bool SpriteComponent_SetAnimationByName(SpriteComponent* component, const char* animName);
