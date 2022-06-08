#pragma once

#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/drawinglayers.h"

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

	// 1.0f is normal speed, 0.5f is half speed,
	// 2.0f is double speed, etc.
	float animationSpeed;

	// Layer on which this sprite will be drawn.
	DrawingLayer layer;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
bool SpriteComponent_SetSpriteSheet(SpriteComponent* component, const char* filePath);
bool SpriteComponent_SetAnimationByName(SpriteComponent* component, const char* animName);
