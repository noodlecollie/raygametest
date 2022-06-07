#pragma once

#include "descriptor/spritesheetdescriptor.h"

void SpriteRenderer_AddRef(void);
void SpriteRenderer_RemoveRef(void);

// Remove me:
void SpriteRenderer_DrawSpriteFrame(SpriteSheetAnimation* animation, size_t frame, Vector2 position, Vector2 scale);

// Rename me:
void SpriteRenderer_DrawSpriteFrameNew(SpriteSheetAnimation* animation, size_t frame, Vector2 position, Vector2 scale);
