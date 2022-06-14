#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "gamelib/external/raylibheaders.h"
#include "gamelib/drawinglayers.h"

struct Entity;
struct SpriteComponentImpl;

typedef enum
{
	SPRITE_TRANS_NONE = 0,
	SPRITE_TRANS_FLIP_X = (1 << 0),
	SPRITE_TRANS_FLIP_Y = (1 << 1),
} SpriteTransformFlag;

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

	int32_t transformFlags;
} SpriteComponent;

struct Entity* SpriteComponent_GetOwnerEntity(const SpriteComponent* component);
bool SpriteComponent_SetSpriteSheet(SpriteComponent* component, const char* filePath);
bool SpriteComponent_SetAnimationByName(SpriteComponent* component, const char* animName);
