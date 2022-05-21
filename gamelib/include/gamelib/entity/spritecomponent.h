#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"

struct Entity;
struct SpriteComponentImpl;

typedef struct SpriteComponent
{
	struct SpriteComponentImpl* impl;

	// Location on sprite that should be
	// placed at the position of the entity.
	Vector2 origin;

	// How big the sprite should be in X and Y,
	// in units per pixel.
	Vector2 scale;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
bool SpriteComponent_SetImage(SpriteComponent* component, const char* filePath);
