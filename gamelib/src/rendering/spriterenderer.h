#pragma once

#include "descriptor/spritesheetdescriptor.h"
#include "gamelib/drawinglayers.h"

void SpriteRenderer_AddRef(void);
void SpriteRenderer_RemoveRef(void);

void SpriteRenderer_DrawSpriteFrame(
	SpriteSheetAnimation* animation,
	size_t frame,
	Vector2 position,
	Vector2 scale,
	DrawingLayer layer,
	uint32_t flags
);
